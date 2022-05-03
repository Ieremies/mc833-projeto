#ifndef MC833_PROJETO_NET_UTILS_H
#define MC833_PROJETO_NET_UTILS_H

#include "movie.h"
#include <netinet/in.h>
#include <sys/socket.h>

#define MAX_STR_LEN 200

typedef enum { POST, GET, PUT, EXIT } Operation;

typedef struct {
    Operation op;
    Movie movie;
} Payload;

void *get_in_addr(struct sockaddr *sa);

#endif // MC833_PROJETO_NET_UTILS_H
