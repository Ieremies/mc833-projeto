#include "../utils/net_utils.h"
#include <stdio.h>
#include <stdlib.h>
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
 * @param[in] socket Description
 */
void post_movie(Movie *movie, int socket) { add_movie(&CATALOG, movie); }

/**
 * @brief Função responsável por atualizar as informações de um filme.
 * @details As informações que vierem preenchidas serão aquelas a serem
 * atualizadas. O filme será determinado pelo ID, único campo obrigatório.
 * @param[in] movie Struct com as informações a serem atualizadas preenchidas e
 * o resto vazio.
 */
void put_movie(Movie *movie, int socket) { update_movie(&CATALOG, movie); }

/**
 * @brief Função responsável por recuperar informações.
 * @details Retorna os filmes que atendem os critérios de filtragem.
 * @return Struct Respos com todo os filmes filtrados.
 */
void get_movie(Movie *movie, int socket) {
    Response response;
    char buf[sizeof(Response)];

    memset(&response, 0, sizeof(Response));
    response.data = CATALOG;
    if (movie->id != ALL)
        filter_by_id(&response.data, movie);
    if (movie->num_genres != 0)
        filter_by_genres(&response.data, movie);
    memcpy(buf, &response, sizeof(Response));

    // Sending procedure:
    size_t aux;
    size_t sent = 0, total = sizeof(response.data.size) +
                             response.data.size * sizeof(Movie);
    while (sent < total) {
        // BUG Esse send pode mudar no UDP
        aux = send(socket, &buf[sent], total - sent, 0);
        if (aux == -1)
            perror("send");
        sent += aux;
    }
}

/**
 * @brief Summary
 * @details Description
 * @param[in] movie Description
 * @param[in] socket Description
 */
void del_movie(Movie *movie, int socket) { delete_movie(&CATALOG, movie); }
/**
 * @}
 */

/**===========================================================================*/
void (*handlers[])(Movie *, int) = {post_movie, get_movie, put_movie,
                                    del_movie};
