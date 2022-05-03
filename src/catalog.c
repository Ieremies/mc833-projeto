#include "catalog.h"
#include <stdlib.h>

Catalog *create_catalog() { return calloc(1, sizeof(Catalog)); }

void add_movie(Catalog *catalog, Movie movie) {
    if (catalog->size < MAX_MOVIES)
        catalog->movie_list[catalog->size++] = movie;
}

void dell_catalog(Catalog *catalog) {
    free(catalog);
}
