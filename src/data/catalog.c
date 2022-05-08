#include "catalog.h"

/**
 * @brief Função para adicionar um filme a um catálogo.
 * @details Caso não haja espaço para adicioná-lo, nada é feito.
 * @param[inout] catalog Catálogo a ser modificado.
 * @param[in] movie Filme a ser adicionado.
 */
void add_movie(Catalog *catalog, Movie movie) {
    if (catalog->size < MAX_MOVIES)
        catalog->movie_list[catalog->size++] = movie;
}

/**
 * @brief Modifica um filme no catálogo com as informações passadas.
 * @details Baseado no movie->id, modifica os dados deste no catálogo.
 * Por enquanto, apenas adição de gênero é suportada.
 * @param[inout] catalog Lista de filmes contendo o filme a ser alterado.
 * @param[in] movie As informações a serem atualizadas estarão preenchidas.
 * O resto deve estar vazio enquanto que o ID é obirgatório.
 */
void update_movie(Catalog *catalog, Movie movie) {
    for (int i = 0; i < catalog->size; i++)
        if (movie.id == catalog->movie_list[i].id) {
            add_genre(&catalog->movie_list[i], movie.genre_list[0]);
            return;
        }
}

/**
 * @brief Deleta um movie baseado no seu ID.
 * @details Para deleção, apenas sobrecrevemo-o.
 * @param[inout] catalog Catálogo da onde o movie será tirado.
 * @param[in] movie Movie com o ID do filme a ser removido.
 */
void delete_movie(Catalog *catalog, Movie movie) {
    for (int i = 0; i < catalog->size; i++)
        if (movie.id == catalog->movie_list[i].id) {
            // FIXME free movie[i];
            for (int j = i; j < catalog->size; i++)
                catalog->movie_list[j] = catalog->movie_list[j + 1];
            catalog->size--;
            return;
        }
}
