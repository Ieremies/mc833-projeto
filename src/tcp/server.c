/*
** server.c -- a stream socket server demo
*/

#include "catalog.h"
#include "movie.h"
#include "net_utils.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

#define PORT "3490" // the port users will be connecting to

#define BACKLOG 10 // how many pending connections queue will hold

int SOCKFD;      // global to be closed on exit
Catalog CATALOG; // global to all handlers task

void sigchld_handler(int s) {
    (void)s; // quiet unused variable warning

    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;
    while (waitpid(-1, NULL, WNOHANG) > 0);
    errno = saved_errno;
}

void post_movie(Movie movie) { add_movie(&CATALOG, movie); }

void put_movie(Movie movie) {
    int i, found = 0;
    for (i = 0; i < CATALOG.size; i++)
        if (movie.id == CATALOG.movie_list[i].id) {
            found = 1;
            break;
        }
    if (!found) { // insert the new movie
        add_movie(&CATALOG, movie);
        return;
    }

    // Update the movie with the new values:
    if (movie.title[0] != '\0')
        memcpy(CATALOG.movie_list[i].title, movie.title, MAX_STR_LEN);
    if (movie.num_genres > 0)
        for (int j = 0; j < movie.num_genres; j++)
            add_genre(&CATALOG.movie_list[i], movie.genre_list[j]);
    if (movie.director_name[0] != '\0')
        memcpy(CATALOG.movie_list[i].director_name, movie.director_name,
               MAX_STR_LEN);
    if (movie.year != 0)
        CATALOG.movie_list[i].year = movie.year;
}

void (*handlers[])(Movie) = {post_movie, post_movie, put_movie, post_movie};

void handle_client(int socket) {
    Payload payload;
    char payload_str[sizeof(Payload)];
    while (1) {
        if (recv(socket, payload_str, sizeof(Payload), 0) == -1)
            perror("recv");

        // Coverts a byte stream to a Payload object:
        memcpy(&payload, payload_str, sizeof(Payload));
        if (payload.op == EXIT)
            break;
        if (payload.op < 0 || payload.op > EXIT) {
            printf("\nInvalid operation\n");
            continue;
        }
#pragma omp critical
        handlers[payload.op](payload.movie); // execute the action
    }
}

void sigint_handler(int sig_num) { // Signal Handler for SIGINT
    close(SOCKFD);
    system("clear");
    printf("server: exiting...\n");
    sleep(1);
    exit(0);
}

int main() {
    system("clear");
    CATALOG.size = 0;
    int new_fd, code; // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes = 1;
    char s[INET6_ADDRSTRLEN];
    int rv;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // Loop through all the results and bind to the first we can:
    for (p = servinfo; p != NULL; p = p->ai_next) {
        SOCKFD = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (SOCKFD == -1) {
            perror("server: socket");
            continue;
        }

        code = setsockopt(SOCKFD, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
        if (code == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(SOCKFD, p->ai_addr, p->ai_addrlen) == -1) {
            close(SOCKFD);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL) {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(SOCKFD, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    // Make sure the socket will be cleaned:
    signal(SIGINT, sigint_handler);

    printf("server: waiting for connections...\n");
#pragma omp parallel
#pragma omp single nowait
    while (1) { // main accept() loop
        sin_size = sizeof(their_addr);
        new_fd = accept(SOCKFD, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family,
                  get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
        printf("server: got connection from %s\n", s);

#pragma omp task firstprivate(new_fd) // this is the child task
        {
            handle_client(new_fd);
            close(new_fd);
        }
    }
}
