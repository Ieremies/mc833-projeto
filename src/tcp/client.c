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

Payload post_movie() {
    Payload ret;
    ret.op = POST;

    system("clear");
    printf("Cadastro de filme");

    printf("\nDigite o id do filme: ");
    scanf("%d", &ret.movie.id);
    getchar(); // ignores the leading \n

    printf("Digite o titulo do filme: ");
    fgets(ret.movie.title, MAX_STR_LEN, stdin);

    printf("Digite o numero de generos desse filme: ");
    scanf("%d", &ret.movie.num_genres);
    getchar(); // ignores the leading \n

    for (int i = 0; i < ret.movie.num_genres; i++) {
        printf("Digite o %dº genero desse filme: ", i + 1);
        fgets(ret.movie.genre_list[i], MAX_STR_LEN, stdin);
    }

    printf("Digite o nome do diretor desse filme: ");
    fgets(ret.movie.director_name, MAX_STR_LEN, stdin);

    printf("Digite o ano desse filme: ");
    scanf("%d", &ret.movie.year);
    getchar(); // ignores the leading \n

    return ret;
}

Payload put_genre() {
    Payload ret;
    ret.op = PUT;

    return ret;
}

Payload (*handlers[])() = {post_movie, put_genre};

void print_menu() {
    system("clear");
    printf("0 - Cadastrar um novo filme");
    printf("\n1 - Acrescentar um novo gênero em um filme");
    printf("\ne - exit");
    printf("\nDigite um comando: ");
}

void handle_user(int socket) {
    char cmd, payload_str[sizeof(Payload)];
    print_menu();
    while (scanf(" %c", &cmd) == 1) {
        if (cmd == 'e')
            break;

        cmd -= '0'; // converts to number
        // Checks if is a valid command:
        if (cmd >= 0 && cmd < sizeof(handlers) / sizeof(void *)) {
            Payload payload = handlers[cmd](socket);

            // Coverts the Payload to a byte stream:
            memcpy(payload_str, &payload, sizeof(Payload));
            if (send(socket, payload_str, sizeof(Payload), 0) == -1)
                perror("send");
        } else
            printf("\nInvalid command\n");
        sleep(1);
        print_menu();
    }

    Payload payload;
    payload.op = EXIT;
    // Coverts the Payload to a byte stream:
    memcpy(payload_str, &payload, sizeof(Payload));
    if (send(socket, payload_str, sizeof(Payload), 0) == -1)
        perror("send");
}

int main(int argc, char *argv[]) {
    int sockfd;
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
