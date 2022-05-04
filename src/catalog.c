#include "catalog.h"

void add_movie(Catalog *catalog, Movie movie) {
    if (catalog->size < MAX_MOVIES)
        catalog->movie_list[catalog->size++] = movie;
}
