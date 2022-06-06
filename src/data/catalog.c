#include "catalog.h"
#include <string.h>

/**
 * @brief Função para adicionar um filme a um catálogo.
 * @details Caso não haja espaço para adicioná-lo, nada é feito.
 * @param[inout] catalog Catálogo a ser modificado.
 * @param[in] movie Filme a ser adicionado.
 */
void add_movie(Catalog *catalog, Movie *movie) {
    if (catalog->size < MAX_MOVIES)
        catalog->movie_list[catalog->size++] = *movie;
}

/**
 * @brief Modifica um filme no catálogo com as informações passadas.
 * @details Baseado no movie->id, modifica os dados deste no catálogo.
 * Por enquanto, apenas adição de gênero é suportada.
 * @param[inout] catalog Lista de filmes contendo o filme a ser alterado.
 * @param[in] movie As informações a serem atualizadas estarão preenchidas.
 * O resto deve estar vazio enquanto que o ID é obirgatório.
 */
void update_movie(Catalog *catalog, Movie *movie) {
    for (int i = 0; i < catalog->size; i++)
        if (movie->id == catalog->movie_list[i].id) {
            for (int j = 0; j < movie->num_genres; j++)
                add_genre(&catalog->movie_list[i], movie->genre_list[j]);
            return;
        }
}

/**
 * @brief Deleta um movie baseado no seu ID.
 * @details Para deleção, apenas sobrecrevemo-o.
 * @param[inout] catalog Catálogo da onde o movie será tirado.
 * @param[in] movie Movie com o ID do filme a ser removido.
 */
void delete_movie(Catalog *catalog, Movie *movie) {
    for (int i = 0; i < catalog->size; i++)
        if (movie->id == catalog->movie_list[i].id) {
            if (i == catalog->size - 1) // Only clear the last position:
                memset(&catalog->movie_list[i], 0, sizeof(Movie));
            else // Move the rest of the array back one position:
                memcpy(&catalog->movie_list[i], &catalog->movie_list[i + 1],
                   (catalog->size - i - 1) * sizeof(Movie));
            catalog->size--;
            return;
        }
}

void filter_by_id(Catalog *catalog, Movie *movie) {
    for (int i = 0; i < catalog->size; i++)
        if (movie->id == catalog->movie_list[i].id) {
            memcpy(&catalog->movie_list[0], &catalog->movie_list[i],
                   sizeof(Movie)); // copy the movie to the first position
            memset(&catalog->movie_list[1], 0,
                   (catalog->size - 1) * sizeof(Movie)); // clear the rest
            catalog->size = 1;
            return;
        }
    catalog->size = 0; // this film is not in the catalog
    memset(catalog->movie_list, 0,
           catalog->size * sizeof(Movie)); // clear the catalog
}

void filter_by_genres(Catalog *catalog, Movie *movie) {
    short size = 0;
    for (int i = 0; i < catalog->size; i++)
        for (int j = 0; j < movie->num_genres; j++)
            if (contains_genre(&catalog->movie_list[i], movie->genre_list[j])) {
                catalog->movie_list[size++] = catalog->movie_list[i];
                break;
            }
    memset(&catalog->movie_list[size], 0,
           (catalog->size - size) * sizeof(Movie)); // clear the rest
    catalog->size = size;
}
