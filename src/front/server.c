#include "../utils/net_utils.h"
#include <malloc.h>
#include <stdio.h>
#include <string.h>

Catalog CATALOG; // global to all handlers task

/**===========================================================================*/
/**
 * @name Interface com o banco de dados.
 * Funções responsáveis por interagir com as informações,
 * alheia de qualquer protocolo de rede.
 * @{
 */

/**
 * @brief Summary
 * @details Description
 * @param[in] movie Description
 */
void *post_movie(Movie *movie) { add_movie(&CATALOG, movie); }

/**
 * @brief Função responsável por atualizar as informações de um filme.
 * @details As informações que vierem preenchidas serão aquelas a serem
 * atualizadas. O filme será determinado pelo ID, único campo obrigatório.
 * @param[in] movie Struct com as informações a serem atualizadas preenchidas e
 * o resto vazio.
 */
void *put_movie(Movie *movie) { update_movie(&CATALOG, movie); }

/**
 * @brief Função responsável por recuperar informações.
 * @details Retorna os filmes que atendem os critérios de filtragem.
 * @return Struct Respos com todo os filmes filtrados.
 */
void *get_movie(Movie *movie) {
    Response *response = calloc(1, sizeof(Response));
    response->data = CATALOG;
    if (movie->id != ALL)
        filter_by_id(&response->data, movie);
    if (movie->num_genres != 0)
        filter_by_genres(&response->data, movie);

    return response;
}

/**
 * @brief Summary
 * @details Description
 * @param[in] movie Description
 */
void *del_movie(Movie *movie) { delete_movie(&CATALOG, movie); }
/**
 * @}
 */

/**===========================================================================*/
void *(*handlers[])(Movie *) = {post_movie, get_movie, put_movie, del_movie};
