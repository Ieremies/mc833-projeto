#include "catalogo.h"
#include <stdlib.h>

#define MAX_NUM_FILMES 256

Catalogo *cria_catalogo() {
    Catalogo *novo = calloc(1, sizeof(Catalogo));
    novo->lista = calloc(MAX_NUM_FILMES, sizeof(Filme *));
    return novo;
}

void adiciona_filme(Catalogo *catalogo, Filme *filme) {
    if (catalogo->qtd < MAX_NUM_FILMES)
        catalogo->lista[catalogo->qtd++] = filme;
}

void deleta_catalogo(Catalogo *catalogo) {
    for (int i = 0; i < catalogo->qtd; i++)
        deletar_filme(catalogo->lista[i]);
    free(catalogo->lista);
    free(catalogo);
}
