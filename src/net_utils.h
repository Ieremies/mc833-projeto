#ifndef MC833_PROJETO_NET_UTILS_H
#define MC833_PROJETO_NET_UTILS_H

#include <netinet/in.h>
#include <sys/socket.h>

void *get_in_addr(struct sockaddr *sa);

#endif // MC833_PROJETO_NET_UTILS_H
