/*
** client.c -- a stream socket client demo
*/

#include "movie.h"
#include "net_utils.h"
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT "3490" // the port client will be connecting to

#define MAXDATASIZE 100 // max number of bytes we can get at once

void post_movie(int socket) {
    Movie movie;
    char movie_str[sizeof(Movie)];

    system("clear");
    printf("Cadastro de filme");

    printf("\nDigite o id do filme: ");
    scanf("%d", &movie.id);
    getchar(); // ignores the leading \n

    printf("Digite o titulo do filme: ");
    fgets(movie.title, MAX_STR_LEN, stdin);

    printf("Digite o numero de generos desse filme: ");
    scanf("%d", &movie.num_genres);
    getchar(); // ignores the leading \n

    for (int i = 0; i < movie.num_genres; i++) {
        printf("Digite o %dº genero desse filme: ", i + 1);
        fgets(movie.genre_list[i], MAX_STR_LEN, stdin);
    }

    printf("Digite o nome do diretor desse filme: ");
    fgets(movie.director_name, MAX_STR_LEN, stdin);

    printf("Digite o ano desse filme: ");
    scanf("%d", &movie.year);
    getchar(); // ignores the leading \n

    // Coverts the Movie to a byte stream:
    memcpy(movie_str, &movie, sizeof(Movie));

    if (send(socket, movie_str, sizeof(Movie), 0) == -1)
        perror("send");
}

void print(int socket) { printf("\nprint_movie running\n"); }

void (*handlers[])(int) = {post_movie, print};

void print_menu() {
    system("clear");
    printf("0 - Cadastrar um novo filme");
    printf("\n1 - print_movie");
    printf("\ne - exit");
    printf("\nDigite um comando: ");
}

void handle_user(int socket) {
    char cmd;
    print_menu();
    while (scanf(" %c", &cmd) == 1) {
        if (cmd == 'e')
            break;

        cmd -= '0'; // converts to number
        // Checks if is a valid command:
        if (cmd >= 0 && cmd < sizeof(handlers) / sizeof(void (*)()))
            handlers[cmd](socket);
        else
            printf("\nInvalid command\n");
        sleep(1);
        print_menu();
    }
}

int main(int argc, char *argv[]) {
    int sockfd, numbytes;
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    if (argc != 2) {
        fprintf(stderr, "usage: client hostname\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // Loop through all the results and connect to the first we can:
    for (p = servinfo; p != NULL; p = p->ai_next) {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            perror("client: connect");
            close(sockfd);
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s,
              sizeof s);
    printf("client: connecting to %s\n", s);
    sleep(1);

    freeaddrinfo(servinfo); // all done with this structure

    handle_user(sockfd);
    close(sockfd);

    return 0;
}
