#include "generos.h"
#include <stdlib.h>

Genero *criar_generos() {
    Genero *novo = calloc(1, sizeof(Genero));
    novo->genero = calloc(NUM_MAX_GENEROS, sizeof(char));
    return novo;
}

void adiciona_generos(Genero *lista_generos, char *genero) {
    if (lista_generos->qtd < NUM_MAX_GENEROS)
        lista_generos->genero[lista_generos->qtd++] = genero;
}

void deleta_generos(Genero *lista_generos) {
    for (int i = 0; i < lista_generos->qtd; i++)
        free(lista_generos->genero[i]);
    free(lista_generos);
}
