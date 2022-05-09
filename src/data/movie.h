#ifndef MC833_PROJETO_MOVIE_H
#define MC833_PROJETO_MOVIE_H

#include "../utils/utils.h"

#define MAX_MOVIE_GENRES 10

/**
 * @struct Movie
 * Struct para armazenar as informações referentes a um filme.
 */
typedef struct {
    int id;                                         /**< Identificador único */
    char title[MAX_STR_LEN];                        /**< Título do filme */
    int num_genres;                                 /**< Quantidade de gêneros na sua lista de gêneros */
    char genre_list[MAX_MOVIE_GENRES][MAX_STR_LEN]; /**< Lista de gêneros do filme */
    char director_name[MAX_STR_LEN];                /**< Nome do diretor */
    int year;                                       /**< Ano de publicação */
    // imagem capa;                                 /**< Description */
} Movie;

Movie *create_movie(int id, char *title, char *genre, char *director, int year);
void add_genre(Movie *movie, char *genre);
char contains_genre(Movie *movie, char *genre);

#endif // MC833_PROJETO_MOVIE_H
