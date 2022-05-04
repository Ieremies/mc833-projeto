#ifndef MC833_PROJETO_CATALOG_H
#define MC833_PROJETO_CATALOG_H

#include "movie.h"

#define MAX_MOVIES 256

typedef struct {
    Movie movie_list[MAX_MOVIES];
    int size;
} Catalog;

void add_movie(Catalog *catalog, Movie movie);

#endif // MC833_PROJETO_CATALOG_H
