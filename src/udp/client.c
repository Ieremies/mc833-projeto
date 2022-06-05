/*
** client.c -- a stream socket client demo
*/

#include "../front/client.c"
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define PORT "4950" // the port users will be connecting to

int SOCKFD;
int START_TIME;

/**
 * @brief Função que aguarda o retorno de operações get.
 * @details Uma vez enviada, a operação GET espera um retorno.
 * @param[in] cmd Qual comando foi enviado.
 * @param[in] p Iformações do servidor.
 */
void handle_get(char cmd, struct addrinfo *p) {
    Response response;
    char buf[sizeof(Response)];
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t addr_len = sizeof(their_addr);
    memset(buf, 0, sizeof(Response));

    // Receiving procedure:
    // receive the size of the response:
    ssize_t aux =
        recvfrom(SOCKFD, &response.data.size, sizeof(response.data.size), 0,
                 (struct sockaddr *)&their_addr, &addr_len);
    if (aux == -1) {
        perror("client: recvfrom");
        exit(1);
    }

    // receive the movie list of the response:
    size_t rec = 0, total = response.data.size * sizeof(Movie);
    while (rec < total) {
        aux = recvfrom(SOCKFD, &buf[rec], total - rec, 0,
                       (struct sockaddr *)&their_addr, &addr_len);
        if (aux == -1) {
            perror("client: recvfrom");
            exit(1);
        }
        rec += aux;
    }
    memcpy(&response.data.movie_list, buf, total);

    get_handlers[cmd](response);

    printf("[%d] : GET response received.\n", (int)time(NULL) - START_TIME);
}

/**
 * @brief Signal handler for SIGINT
 */
void sigint_handler(int sig_num) {
    system("clear");
    printf("client: exiting...\n");
    close(SOCKFD);
    sleep(1);
    exit(sig_num);
}

/**
 * @brief Função de controle do menu.
 * @param[in] p Iformações do servidor.
 * @details A cada iteração do menu, lemos um caracter que indica qual o comando
 * a ser realizado.
 */
void handle_user(struct addrinfo *p) {
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
            ssize_t numbytes = sendto(SOCKFD, &payload, sizeof(Payload), 0,
                                      p->ai_addr, p->ai_addrlen);
            if (numbytes == -1) {
                perror("client: sendto");
                sigint_handler(1);
            }
            if (payload.op == GET)
                handle_get(cmd, p);
        } else {
            printf("\nInvalid command\n");
            sleep(1);
        }
        print_menu();
    }
}

int main(int argc, char *argv[]) {
    struct addrinfo hints, *servinfo, *p;
    int rv;

    START_TIME = time(NULL);
    printf("[%d] : Client started.\n", (int)time(NULL) - START_TIME);

    if (argc != 2) {
        fprintf(stderr, "usage: client hostname\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET6; // set to AF_INET to use IPv4
    hints.ai_socktype = SOCK_DGRAM;

    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        exit(1);
    }

    // Loop through all the results and connect to the first we can:
    for (p = servinfo; p != NULL; p = p->ai_next) {
        SOCKFD = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (SOCKFD == -1) {
            perror("client: socket");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to create socket\n");
        exit(2);
    }

    freeaddrinfo(servinfo); // all done with this structure

    // Make sure the socket will be cleaned and the user will send an EXIT:
    signal(SIGINT, sigint_handler);

    handle_user(p);
    close(SOCKFD);

    return 0;
}
