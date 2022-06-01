#include "../utils/net_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
 * @brief Listar todas as informações de todos os títulos.
 * @details
 * @return Struct Payload com as informações buscadas
 */
Payload get_movies() {
    Payload ret;
    memset(&ret, 0, sizeof(Payload));
    ret.op = GET;
    ret.movie.id = ALL;
    return ret;
}

/**
 * @brief Listar todas as informações de todos os títulos com certo gênero.
 * @details
 * @return Struct Payload com as informações buscadas
 */
Payload get_movies_by_genre() {
    Payload ret;
    memset(&ret, 0, sizeof(Payload));
    ret.op = GET;
    ret.movie.id = ALL;

    system("clear");
    printf("Digite o gênero: ");
    ret.movie.num_genres = 1;
    fgets(ret.movie.genre_list[0], MAX_STR_LEN, stdin);
    ret.movie.genre_list[0][strcspn(ret.movie.genre_list[0], "\n")] = '\0';

    return ret;
}

/**
 * @brief Listar todas as informações de um único filme.
 * @details
 * @return Struct Payload com as informações buscadas
 */
Payload get_movie() {
    Payload ret;
    memset(&ret, 0, sizeof(Payload));
    ret.op = GET;

    system("clear");
    printf("Digite o id: ");
    scanf("%hd", &ret.movie.id);
    getchar(); // ignores the leading \n

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
    scanf("%hd", &ret.movie.id);
    getchar(); // ignores the leading \n

    printf("Digite o título do filme: ");
    fgets(ret.movie.title, MAX_STR_LEN, stdin);
    ret.movie.title[strcspn(ret.movie.title, "\n")] = '\0';

    printf("Digite o número de gêneros do filme: ");
    scanf("%hd", &ret.movie.num_genres);
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
    scanf("%hd", &ret.movie.year);
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
    scanf("%hd", &ret.movie.id);

    printf("Digite o número de gêneros que deseja adicionar a esse filme: ");
    scanf("%hd", &ret.movie.num_genres);
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
    scanf("%hd", &ret.movie.id);
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
 * @details Imprime no formato "-> ID - TÍTULO\n".
 * @param[in] response Resposta com o catálogo a ser impresso.
 */
void list_titles(Response response) {
    system("clear");
    printf("Lista de filmes:\n");
    printf(" -> id  - título\n");
    for (int i = 0; i < response.data.size; i++)
        printf(" -> %d - %s\n", response.data.movie_list[i].id,
               response.data.movie_list[i].title);
    wait_for_enter();
}

/**
 * @brief Imprime todas as informações de um filme.
 * @details Fotmato " -> ID - ANO TÍTULO DIRETOR | GÊNEROS".
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
    for (int i = 0; i < response.data.size; i++)
        print_all_info(response.data.movie_list[i]);
    wait_for_enter();
}

/**
 * @brief Função para imprimir apenas os filmes de um gênero.
 * @details A partir do catálogo solicitado ao servidor, imprimimos apenas
 * aqueles que possuem em sua lista de gêneros, o genero fornecido.
 * @param[in] response Resposta do servidor com o catálogo.
 */
void list_info_by_genre(Response response) {
    system("clear");
    printf("Lista de filmes:\n");
    printf(" -> título - nome do diretor - ano\n");
    Movie aux;
    for (int i = 0; i < response.data.size; i++) {
        aux = response.data.movie_list[i];
        printf(" -> %s - %s - %d\n", aux.title, aux.director_name, aux.year);
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
    system("clear");
    printf(" -> id  - (ano) título by diretor | Gêneros\n");
    print_all_info(response.data.movie_list[0]);
    wait_for_enter();
}
/**
 * @}
 */
/**===========================================================================*/

Payload (*handlers[])() = {post_movie,          put_genre,  get_movies,
                           get_movies_by_genre, get_movies, get_movie,
                           remove_movie};
void (*get_handlers[])(Response) = {NULL,          NULL,
                                    list_titles,   list_info_by_genre,
                                    list_all_info, list_info_by_id};

/**
 * @brief Impressão do menu principal para o cliente.
 */
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
