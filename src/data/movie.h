#ifndef MC833_PROJETO_MOVIE_H
#define MC833_PROJETO_MOVIE_H

#include "../utils/utils.h"

#define MAX_MOVIE_GENRES 4

/**
 * @struct Movie
 * Struct para armazenar as informações referentes a um filme.
 */
typedef struct {
    short id;                                       /**< Identificador único */
    char title[MAX_STR_LEN];                        /**< Título do filme */
    short num_genres;                               /**< Quantidade de gêneros na sua lista de gêneros */
    char genre_list[MAX_MOVIE_GENRES][MAX_STR_LEN]; /**< Lista de gêneros do filme */
    char director_name[MAX_STR_LEN];                /**< Nome do diretor */
    short year;                                     /**< Ano de publicação */
    // imagem capa;                                 /**< Description */
} Movie;

Movie *create_movie(short id, char *title, char *genre, char *director, short year);
void add_genre(Movie *movie, char *genre);
char contains_genre(Movie *movie, char *genre);

#endif // MC833_PROJETO_MOVIE_H
