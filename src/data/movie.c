#include "movie.h"
#include <stdlib.h>
#include <string.h>

/**
 * @brief Função para criar uma struct movie.
 * @details Aloca um espaço em memória para a struct e preenche com os dados
 * passados.
 * @param[in] id
 * @param[out] title Título do filme a ser criado.
 * @param[out] genre Gêneros iniciais para o filme.
 * @param[out] director Nome do diretor.
 * @param[in] year Ano de publicação
 * @return Ponteiro para a nova struct movie.
 */
Movie *create_movie(int id, char *title, char *genre, char *director,
                    int year) {
    Movie *new = calloc(1, sizeof(Movie));
    new->id = id; // TODO acho que temos que mudar esse compostamento
    memcpy(new->title, title, MAX_STR_LEN);
    memcpy(new->genre_list[new->num_genres++], genre, MAX_STR_LEN);
    memcpy(new->director_name, director, MAX_STR_LEN);
    new->year = year;
    return new;
}

/**
 * @brief Adiciona um gênero a lista de gêneros do filme.
 * @param[inout] movie Filme a ter um gênero inserido.
 * @param[out] genre Gênero a ser inserido.
 */
void add_genre(Movie *movie, char *genre) {
    memcpy(movie->genre_list[movie->num_genres++], genre, MAX_STR_LEN);
}

/**
 * @brief Confere se o gênero está na lista de gêneros.
 * @details Usa comparação de strings para determinar se o gênero fornecido está
 * ou não presente no filme.
 * @param[inout] movie Filme a ser testado.
 * @param[out] genre Genero a ser procurado
 * @return 1 caso o filme seja do gênero passado, 0 caso contrário.
 */
char contains_genre(Movie *movie, char *genre) {
    for (int i = 0; i < movie->num_genres; i++)
        if (strstr(movie->genre_list[i], genre))
            return 1;
    return 0;
}
