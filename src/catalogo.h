#include "filme.h"

typedef struct {
    Filme **lista;
    int qtd;
} Catalogo;

Catalogo *cria_catalogo();
void adiciona_filme(Catalogo *catalogo, Filme *filme);
void deleta_catalogo(Catalogo *catalogo);
