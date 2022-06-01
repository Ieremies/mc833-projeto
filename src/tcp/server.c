/*
** server.c -- a stream socket server demo
*/

#include "../front/server.c"
#include "../data/catalog.h"
#include "../utils/net_utils.h"
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

int SOCKFD; // global to be closed on exit

void sigchld_handler(int s) {
    (void)s; // quiet unused variable warning

    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;
    while (waitpid(-1, NULL, WNOHANG) > 0)
        ;
    errno = saved_errno;
}

/**
 * @brief Função que espera por uma conexão.
 * @details Em um laço infinito, esperamos uma nova conexão chegar.
 * @param[in] socket Socket a escutar.
 */
void handle_client(int socket) {
    Payload payload;
    while (1) {
        if (recv(socket, &payload, sizeof(Payload), 0) == -1)
            perror("recv");

        if (payload.op == EXIT)
            break;
        if (payload.op < 0 || payload.op > EXIT) {
            printf("\nInvalid operation\n");
            continue;
        }
#pragma omp critical(Catalog)
        handlers[payload.op](&payload.movie, socket); // execute the action
    }
}

/**
 * @brief Função para salvar o estado do catálogo.
 * @details Escrevemos o estado atual a um arquivo para arquiva-lo.
 */
void backup() {
    FILE *f = fopen("catalog_database.data", "wb");
    if (f == NULL) {
        printf("\nError opening database file\n");
        return;
    }

    char catalog_str[sizeof(Catalog)];
    // Coverts the Catalog to a byte stream:
#pragma omp critical(Catalog)
    memcpy(catalog_str, &CATALOG, sizeof(Catalog));
    fwrite(catalog_str, sizeof(Catalog), 1, f);
    fclose(f);
}

/**
 * @brief Função para recuperar o backup.
 * @details Lemos o arquivo que foi salvo anteriormente para recuperar o
 * catálogo.
 */
void load_backup() {
    FILE *f = fopen("catalog_database.data", "rb");
    if (f == NULL) {
        printf("\nError opening database file\n");
        return;
    }

    fread(&CATALOG, sizeof(Catalog), 1, f);
    fclose(f);
}

/**
 * @brief Função para finalizar o servidor
 * @details Fechamos o socket, fazemos o backup do catálogo e terminamos o
 * programa.
 */
void sigint_handler(int sig_num) {
    close(SOCKFD);
    system("clear");
    printf("server: exiting...\n");
    backup();
    sleep(1);
    exit(0);
}

int main(int argc, char *argv[]) {
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

    if (argc == 2 && strcmp(argv[1], "load") == 0)
        load_backup();

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // either ipv4 or ipv6
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

        // Check if the socket is ALL clear to be used.
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
#pragma omp critical(Backup)
            backup();
        }
    }
}
