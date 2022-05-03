#include "movie.h"

#define MAX_MOVIES 256

typedef struct {
    Movie movie_list[MAX_MOVIES];
    int size;
} Catalog;

Catalog *create_catalog();
void add_movie(Catalog *catalog, Movie movie);
void dell_catalog(Catalog *catalog);
