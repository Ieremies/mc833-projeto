#include "movie.h"
#include <stdlib.h>
#include <string.h>

Movie *create_movie(int id, char *title, char *genre, char *director,
                    int year) {
    Movie *new = calloc(1, sizeof(Movie));
    new->id = id;
    memcpy(new->title, title, MAX_STR_LEN);
    memcpy(new->genre_list[new->num_genres++], genre, MAX_STR_LEN);
    memcpy(new->director_name, director, MAX_STR_LEN);
    new->year = year;
    return new;
}

void add_genre(Movie *movie, char *genre) {
    memcpy(movie->genre_list[movie->num_genres++], genre, MAX_STR_LEN);
}
