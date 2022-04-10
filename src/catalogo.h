#import "filme.h"

typedef struct {
    Filme **lista;
    int qtd;
} Catalogo;

Catalogo *criar_catalogo();
void adicionar_filme(Catalogo *catalogo, Filme *filme);
void deletar_catalogo(Catalogo *catalogo);
