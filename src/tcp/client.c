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

void wait_for_enter() {
    static char aux[MAX_STR_LEN];
    printf("Aperte enter para continuar...");
    fgets(aux, MAX_STR_LEN, stdin);
}

/**===========================================================================*/
/**
 * @name Operações do servidor
 * Funções para montar o payload compatível com as operações do servidor.
 * @{
 */
/**
 * @brief Listar todos os títulos, junto aos seus respectivos identificadores.
 * @details
 * @return Struct Payload com as informações a serem colocadas
 */
Payload get_movies() {
    Payload ret;
    memset(&ret, 0, sizeof(Payload));
    ret.op = GET;
    ret.movie.id = ALL;
    return ret;
}

/**
 * @brief Cadastrar um novo filme.
 * @details O identificador numérico será definido pelo sistema
 * @return Struct Payload com as informações a serem colocadas.
 */
Payload post_movie() {
    Payload ret;
    memset(&ret, 0, sizeof(Payload));
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

/**
 * @brief Acrescentar um novo gênero a um filme
 * @details
 * @return Struct Payload com as informações a serem colocadas.
 */
Payload put_genre() {
    Payload ret;
    memset(&ret, 0, sizeof(Payload));
    ret.op = PUT;

    system("clear");
    printf("Adição de gênero em filme");

    printf("\nDigite o id do filme: ");
    scanf("%d", &ret.movie.id);

    // BUG Não precisa ser mais de um, caso dê problema sugiro simplificarmos.
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

/**
 * @brief Remover um filme do catálogo a partir de seu ID
 * @details Description
 * @return Description
 */
Payload remove_movie() {
    Payload ret;
    memset(&ret, 0, sizeof(Payload));
    ret.op = DEL;

    system("clear");
    printf("Remover filme");

    printf("\nDigite o id do filme a ser removido: ");
    scanf("%d", &ret.movie.id);
    getchar(); // ignores the leading \n

    return ret;
}
/**
 * @}
 */
/**===========================================================================*/
/**
 * @name Funções de impressão
 * Funções auxiliares para imprimir os resultados obtidos.
 * @{
 */
/**
 * @brief Imprimir os filmes obtidos.
 * @details Imprime no formato "-> ID - TÌTULO\n".
 * @param[in] response Resposta com o catálogo a ser impresso.
 */
void list_titles(Response response) {
    system("clear");
    printf("Lista de filmes:\n");
    printf(" -> id  - título\n");
    for (int i = 0; i < response.data.catalog.size; i++)
        printf(" -> %d - %s\n", response.data.catalog.movie_list[i].id,
               response.data.catalog.movie_list[i].title);
    wait_for_enter();
}

/**
 * @brief Imprime todas as informações de um filme.
 * @details Fotmato " -> ID - ANO TÌTULO DIRETOR | GÊNEROS".
 * @param[in] movie Filme cujas informações devemos imprimir.
 */
void print_all_info(Movie movie) {
    printf(" -> %d - (%d) %s by %s |", movie.id, movie.year, movie.title,
           movie.director_name);
    for (int i = 0; i < movie.num_genres; i++)
        printf(" %s", movie.genre_list[i]);
    printf("\n");
}

/**
 * @brief Summary
 * @details Description
 * @param[in] response Description
 */
void list_all_info(Response response) {
    system("clear");
    printf("Informações dos filmes:\n");
    printf(" -> id  - (ano) título by diretor | Gêneros\n");
    for (int i = 0; i < response.data.catalog.size; i++)
        print_all_info(response.data.catalog.movie_list[i]);
    wait_for_enter();
}

/**
 * @brief Função para imprimir apenas os filmes de um gênero.
 * @details A partir do catálogo solicitado ao servidor, imprimimos apenas
 * aqueles que possuem em sua lista de gêneros, o genero fornecido.
 * @param[in] response Resposta do servidor com o catálogo.
 */
void list_info_by_genre(Response response) {
    char genre[MAX_STR_LEN];
    system("clear");
    printf("Digite o gênero: ");
    fgets(genre, MAX_STR_LEN, stdin);
    genre[strcspn(genre, "\n")] = '\0';

    // REVIEW Do jeito que ele colocou na descrição, parecia que ele queria
    // que essa filtragem fosse feita pelo servidor.
    system("clear");
    printf("Lista de filmes:\n");
    printf(" -> título - nome do diretor - ano\n");
    Movie aux;
    for (int i = 0; i < response.data.catalog.size; i++) {
        aux = response.data.catalog.movie_list[i];
        if (contains_genre(&aux, genre))
            printf(" -> %s - %s - %d\n", aux.title, aux.director_name,
                   aux.year);
    }
    wait_for_enter();
}

/**
 * @brief Função para imprimir apenas o filme com um id.
 * @details A partir do catálogo solicitado ao servidor, imprimimos apenas
 * aquele que possue id igual ao fornecido.
 * @param[in] response Resposta do servidor com o catálogo.
 */
void list_info_by_id(Response response) {
    int id;
    system("clear");
    printf("Digite o id: ");
    scanf("%d", &id);
    getchar(); // ignores the leading \n

    system("clear");
    printf(" -> id  - (ano) título by diretor | Gêneros\n");
    for (int i = 0; i < response.data.catalog.size; i++)
        if (response.data.catalog.movie_list[i].id == id) {
            print_all_info(response.data.catalog.movie_list[i]);
            break;
        }

    wait_for_enter();
}
/**
 * @}
 */

/**===========================================================================*/
Payload (*handlers[])() = {post_movie, put_genre,  get_movies,  get_movies,
                           get_movies, get_movies, remove_movie};
void (*get_handlers[])(Response) = {NULL,          NULL,
                                    list_titles,   list_info_by_genre,
                                    list_all_info, list_info_by_id};

void print_menu() {
    system("clear");
    printf("0 - Cadastrar um novo filme");
    printf("\n1 - Acrescentar um novo gênero em um filme");
    printf("\n2 - Listar títulos");
    printf("\n3 - Listar informações por gênero");
    printf("\n4 - Listar todas as informações de todos os filmes");
    printf("\n5 - Listar todas as informações de um filme");
    printf("\n6 - Remover filme");
    printf("\ne - exit");
    printf("\nDigite um comando: ");
}

void send_exit() {
    Payload payload;
    payload.op = EXIT;
    if (send(SOCKFD, &payload, sizeof(Payload), 0) == -1)
        perror("send");
}

void handle_get(char cmd) {
    Response response;
    if (recv(SOCKFD, &response, sizeof(Response), 0) == -1)
        perror("recv");
    get_handlers[cmd](response);
}

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
