/*
** client.c -- a stream socket client demo
*/

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

#define PORT "3490" // the port client will be connecting to

int SOCKFD;

Payload post_movie() {
    Payload ret;
    ret.op = POST;

    system("clear");
    printf("Cadastro de filme");

    printf("\nDigite o id do filme: ");
    scanf("%d", &ret.movie.id);
    getchar(); // ignores the leading \n

    printf("Digite o título do filme: ");
    fgets(ret.movie.title, MAX_STR_LEN, stdin);
    ret.movie.title[strcspn(ret.movie.title, "\n")] = '\0';

    printf("Digite o número de gêneros do filme: ");
    scanf("%d", &ret.movie.num_genres);
    getchar(); // ignores the leading \n

    for (int i = 0; i < ret.movie.num_genres; i++) {
        printf("Digite o %dº gênero do filme: ", i + 1);
        fgets(ret.movie.genre_list[i], MAX_STR_LEN, stdin);
        ret.movie.genre_list[i][strcspn(ret.movie.genre_list[i], "\n")] = '\0';
    }

    printf("Digite o nome do diretor do filme: ");
    fgets(ret.movie.director_name, MAX_STR_LEN, stdin);
    ret.movie.director_name[strcspn(ret.movie.director_name, "\n")] = '\0';

    printf("Digite o ano do filme: ");
    scanf("%d", &ret.movie.year);
    getchar(); // ignores the leading \n

    return ret;
}

Payload put_genre() {
    Payload ret;
    memset(&ret, 0, sizeof(Payload));
    ret.op = PUT;

    system("clear");
    printf("Adição de gênero em filme");

    printf("\nDigite o id do filme: ");
    scanf("%d", &ret.movie.id);

    printf("Digite o número de gêneros que deseja adicionar a esse filme: ");
    scanf("%d", &ret.movie.num_genres);
    getchar(); // ignores the leading \n

    for (int i = 0; i < ret.movie.num_genres; i++) {
        printf("Digite o %dº novo gênero desse filme: ", i + 1);
        fgets(ret.movie.genre_list[i], MAX_STR_LEN, stdin);
        ret.movie.genre_list[i][strcspn(ret.movie.genre_list[i], "\n")] = '\0';
    }

    return ret;
}

Payload get_movies() {
    Payload ret;
    memset(&ret, 0, sizeof(Payload));
    ret.op = GET;
    ret.movie.id = ALL;
    return ret;
}

void list_titles(Response response) {
    system("clear");
    printf("Lista de filmes:\n");
    printf(" -> id  - título\n");
    for (int i = 0; i < response.data.catalog.size; i++)
        printf(" -> %d - %s", response.data.catalog.movie_list[i].id,
               response.data.catalog.movie_list[i].title);
    printf("Aperte enter para continuar...");
    getchar();
    getchar();
}

void list_info_by_genre(Response response) {
    char genre[MAX_STR_LEN];
    system("clear");
    getchar(); // ignores the leading \n
    printf("Digite o gênero: ");
    fgets(genre, MAX_STR_LEN, stdin);
    genre[strcspn(genre, "\n")] = '\0';

    printf("\nLista de filmes:\n");
    printf(" -> título - nome do diretor - ano\n");
    Movie aux;
    for (int i = 0; i < response.data.catalog.size; i++) {
        aux = response.data.catalog.movie_list[i];
        if (contains_genre(&aux, genre))
            printf(" -> %s - %s - %d", aux.title, aux.director_name, aux.year);
    }
    printf("Aperte enter para continuar...");
    getchar();
}

Payload (*handlers[])() = {post_movie, put_genre, get_movies, get_movies};
void (*get_handlers[])(Response) = {NULL, NULL, list_titles,
                                    list_info_by_genre};

void print_menu() {
    system("clear");
    printf("0 - Cadastrar um novo filme");
    printf("\n1 - Acrescentar um novo gênero em um filme");
    printf("\n2 - Listar títulos");
    printf("\n3 - Listar informações por gênero");
    printf("\ne - exit");
    printf("\nDigite um comando: ");
}

void send_exit() {
    Payload payload;
    payload.op = EXIT;
    char payload_str[sizeof(Payload)];
    // Coverts the Payload to a byte stream:
    memcpy(payload_str, &payload, sizeof(Payload));
    if (send(SOCKFD, payload_str, sizeof(Payload), 0) == -1)
        perror("send");
}

void handle_get(char cmd) {
    Response response;
    char response_str[sizeof(Response)];
    if (recv(SOCKFD, response_str, sizeof(Response), 0) == -1)
        perror("recv");
    // Coverts a byte stream to a Response object:
    memcpy(&response, response_str, sizeof(Response));
    get_handlers[cmd](response);
}

void handle_user() {
    char cmd, payload_str[sizeof(Payload)];
    print_menu();
    while (scanf(" %c", &cmd) == 1) {
        if (cmd == 'e')
            break;

        cmd -= '0'; // converts to number
        // Checks if is a valid command:
        if (cmd >= 0 && cmd < sizeof(handlers) / sizeof(void *)) {
            Payload payload = handlers[cmd]();

            // Coverts the Payload to a byte stream:
            memcpy(payload_str, &payload, sizeof(Payload));
            if (send(SOCKFD, payload_str, sizeof(Payload), 0) == -1)
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

void sigint_handler(int sig_num) { // Signal Handler for SIGINT
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
