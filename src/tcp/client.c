/*
** client.c -- a stream socket client demo
*/

#include "../front/client.c"
#include "../utils/net_utils.h"
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT "3490" // the port client we will be connecting to

int SOCKFD;

/**
 * @brief Função para enviar o fechamento de conexão.
 * @details Envia um payload com a operação EXIT.
 */
void send_exit() {
    Payload payload;
    payload.op = EXIT;
    if (send(SOCKFD, &payload, sizeof(Payload), 0) == -1)
        perror("send");
}

/**
 * @brief Função que aguarda o retorno de operações get.
 * @details Uma vez enviada, a operação GET espera um retorno.
 * @param[in] cmd Qual comando foi enviado.
 */
void handle_get(char cmd) {
    Response response;
    char buf[sizeof(Response)];
    memset(buf, 0, sizeof(Response));

    // Receiving procedure:
    // receive the size of the response:
    ssize_t aux;
    recv(SOCKFD, &response.data.size, sizeof(response.data.size), 0);
    if (aux == -1)
        perror("recv");

    // receive the movie list of the response:
    size_t rec = 0, total = response.data.size * sizeof(Movie);
    while (rec < total) {
        aux = recv(SOCKFD, &buf[rec], total - rec, 0);
        if (aux == -1)
            perror("recv");
        rec += aux;
    }
    memcpy(&response.data.movie_list, buf, total);

    get_handlers[cmd](response);
}

/**
 * @brief Função de controle do menu.
 * @details A cada iteração do menu, lemos um caracter que indica qual o comando
 * a ser realizado.
 */
void handle_user() {
    char cmd;
    print_menu();
    while (scanf("%c", &cmd) == 1) {
        getchar(); // ignores the leading \n
        if (cmd == 'e')
            break;

        cmd -= '0'; // converts to number
        // Checks if is a valid command:
        if (cmd >= 0 && cmd < sizeof(handlers) / sizeof(void *)) {
            Payload payload = handlers[cmd]();
            if (send(SOCKFD, &payload, sizeof(Payload), 0) == -1)
                perror("send");
            if (payload.op == GET)
                handle_get(cmd);
        } else {
            printf("\nInvalid command\n");
            sleep(1);
        }
        print_menu();
    }

    send_exit();
}

/**
 * @brief Signal handler for SIGINT
 */
void sigint_handler(int sig_num) {
    system("clear");
    printf("client: exiting...\n");
    send_exit();
    close(SOCKFD);
    sleep(1);
    exit(0);
}

int main(int argc, char *argv[]) {
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
        SOCKFD = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (SOCKFD == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(SOCKFD, p->ai_addr, p->ai_addrlen) == -1) {
            perror("client: connect");
            close(SOCKFD);
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

    // Make sure the socket will be cleaned and the user will send an EXIT:
    signal(SIGINT, sigint_handler);

    handle_user();
    close(SOCKFD);

    return 0;
}
