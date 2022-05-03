#include "movie.h"
#include <stdio.h>
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

void print_movie(Movie *movie) {
    printf("%d %s %s %d\n", movie->id, movie->title, movie->director_name,
           movie->year);
    // TODO print genre_list!
}

void print_title_id(Movie *movie) {
    printf("%d %s\n", movie->id, movie->title);
}
