/*
** server.c -- a stream socket server demo
*/

#include "../front/server.c"
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define PORT "4950" // the port users will be connecting to

int SOCKFD; // global to be closed on exit
int START_TIME;

/**
 * @brief Função que cuida da requisição de um cliente.
 * @param[in] p Informações do cliente.
 */
void handle_client(struct addrinfo *p) {
    Payload payload;
    Response *response;
    char buf[sizeof(Response)];
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t addr_len = sizeof(their_addr);

    ssize_t numbytes = recvfrom(SOCKFD, &payload, sizeof(Payload), 0,
                                (struct sockaddr *)&their_addr, &addr_len);
    if (numbytes == -1) {
        printf("recvfrom error.\n");
        return;
    }
    printf("[%d] : Payload received.\n", (int)time(NULL) - START_TIME);

    if (payload.op < 0 || payload.op > EXIT) {
        printf("\nInvalid operation.\n");
        return;
    }

    response = handlers[payload.op](&payload.movie); // execute the action

    if (payload.op == GET) { // Sending procedure:
        // First send the size of the movie list:
        size_t aux =
            sendto(SOCKFD, &response->data.size, sizeof(response->data.size), 0,
                   (struct sockaddr *)&their_addr, addr_len);
        if (numbytes == -1) {
            printf("sendto error.\n");
            return;
        }

        // Send the full movie list:
        size_t sent = 0, total = response->data.size * sizeof(Movie);
        memcpy(buf, response->data.movie_list, total);
        free(response);
        while (sent < total) {
            aux = sendto(SOCKFD, &buf[sent], total - sent, 0,
                         (struct sockaddr *)&their_addr, addr_len);
            if (numbytes == -1) {
                printf("sendto error.\n");
                return;
            }
            sent += aux;
        }
    }
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
    backup(&CATALOG);
    sleep(1);
    exit(sig_num);
}

int main(int argc, char *argv[]) {
    system("clear");
    CATALOG.size = 0;
    struct addrinfo hints, *servinfo, *p;
    int rv;

    START_TIME = time(NULL);
    printf("[%d] : Server started.\n", (int)time(NULL) - START_TIME);

    if (argc == 2 && strcmp(argv[1], "load") == 0)
        load_backup(&CATALOG);

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // set to AF_INET to use IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        exit(1);
    }

    // Loop through all the results and bind to the first we can:
    for (p = servinfo; p != NULL; p = p->ai_next) {
        SOCKFD = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (SOCKFD == -1) {
            perror("server: socket");
            continue;
        }

        if (bind(SOCKFD, p->ai_addr, p->ai_addrlen) == -1) {
            close(SOCKFD);
            perror("server: bind");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "server: failed to bind socket\n");
        exit(2);
    }

    freeaddrinfo(servinfo); // all done with this structure

    printf("server: waiting for connections...\n");

    // Make sure the socket will be cleaned:
    signal(SIGINT, sigint_handler);

    while (1) {
        handle_client(p);
        backup(&CATALOG);
    }
}
