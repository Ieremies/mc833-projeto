#ifndef MC833_PROJETO_CATALOG_H
#define MC833_PROJETO_CATALOG_H

#include "movie.h"

#define MAX_MOVIES 20

/**
 * @struct Catalog
 * Lista de filmes (movies).
 */
typedef struct {
    char size;                    /**< Quantidade de filmes na lista */
    Movie movie_list[MAX_MOVIES]; /**< Lista que armazena os filmes */
} Catalog; // **SHOULD NOT EXCED 4096 BYTES!!!**

void add_movie(Catalog *catalog, Movie movie);
void update_movie(Catalog *catalog, Movie movie);
void delete_movie(Catalog *catalog, Movie movie);

#endif // MC833_PROJETO_CATALOG_H
