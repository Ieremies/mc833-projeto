#include "utils.h"
#include "data/catalog.h"
#include <bits/types/FILE.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>

/**
 * @brief Função para salvar o estado do catálogo.
 * @details Escrevemos o estado atual a um arquivo para arquiva-lo.
 */
void backup(void *__restrict src_ptr) {
    FILE *f = fopen("catalog_database.data", "wb");
    if (f == NULL) {
        printf("\nError opening database file\n");
        return;
    }

    char catalog_str[sizeof(Catalog)];
    // Coverts the Catalog to a byte stream:
#pragma omp critical(Catalog)
    memcpy(catalog_str, src_ptr, sizeof(Catalog));
    fwrite(catalog_str, sizeof(Catalog), 1, f);
    fclose(f);
}

/**
 * @brief Função para recuperar o backup.
 * @details Lemos o arquivo que foi salvo anteriormente para recuperar o
 * catálogo.
 */
void load_backup(void *__restrict dest_ptr) {
    FILE *f = fopen("catalog_database.data", "rb");
    if (f == NULL) {
        printf("\nError opening database file\n");
        return;
    }

    fread(dest_ptr, sizeof(Catalog), 1, f);
    fclose(f);
}

void sigchld_handler(int socket) {
    (void)socket; // quiet unused variable warning

    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;
    while (waitpid(-1, NULL, WNOHANG) > 0);
    errno = saved_errno;
}
