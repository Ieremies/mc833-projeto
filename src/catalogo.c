#include "catalogo.h"
#include <stdlib.h>

#define MAX_NUM_FILMES 256

Catalogo *criar_catalogo() {
    Catalogo *novo = calloc(1, sizeof(Catalogo));
    novo->lista = calloc(MAX_NUM_FILMES, sizeof(Filme *));
    novo->qtd = 0;
    return novo;
}
void adicionar_filme(Catalogo *catalogo, Filme *filme) {
    if (catalogo->qtd < MAX_NUM_FILMES) {
        catalogo->lista[catalogo->qtd] = filme;
        catalogo->qtd++;
    }
}
void deletar_catalogo(Catalogo *catalogo) {
    for (int i = 0; i < catalogo->qtd; i++)
        delete_filme(catalogo->lista[i]);
    free(catalogo);
}
