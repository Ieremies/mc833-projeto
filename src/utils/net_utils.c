#include "net_utils.h"

// Get sockaddr, IPv4 or IPv6:

/**
 * @brief Retorna o endereço do socket, IPv4 ou IPv6
 * @param[inout] sa FIXME Description
 * @return FIXME ?
 */
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET)
        return &(((struct sockaddr_in *)sa)->sin_addr);
    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}
