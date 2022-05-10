#ifndef MC833_PROJETO_NET_UTILS_H
#define MC833_PROJETO_NET_UTILS_H

#include "../data/catalog.h"
#include "../data/movie.h"
#include <netinet/in.h>
#include <sys/socket.h>

#define ALL -1

typedef enum { POST, GET, PUT, DEL, EXIT } Operation;

/**
 * @struct Payload
 * Struct utilizada para enviar as informações para o servidor.
 */
typedef struct {
    Operation op; /**< Tipo de operação a ser feito. */
    Movie movie; /**< Informações de filme a serem utilizadas pela operação */
} Payload;

typedef union {
    Catalog catalog;
    Movie movie;
} CatalogMovie;

/**
 * @struct Response
 * Struct utilizada como respota do servidor.
 */
typedef struct {
    CatalogMovie data; /**< Tipo de data a ser retornada. */
} Response;

void *get_in_addr(struct sockaddr *sa);

#endif // MC833_PROJETO_NET_UTILS_H
