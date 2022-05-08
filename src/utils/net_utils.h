#ifndef MC833_PROJETO_NET_UTILS_H
#define MC833_PROJETO_NET_UTILS_H

#include "../data/catalog.h"
#include "../data/movie.h"
#include <netinet/in.h>
#include <sys/socket.h>

#define ALL -1

typedef enum { POST, GET, PUT, DEL, EXIT } Operation;

typedef struct {
    Operation op;
    Movie movie;
} Payload;

typedef union {
    Catalog catalog;
    Movie movie;
} CatalogMovie;

typedef struct {
    CatalogMovie data;
} Response;

void *get_in_addr(struct sockaddr *sa);

#endif // MC833_PROJETO_NET_UTILS_H
