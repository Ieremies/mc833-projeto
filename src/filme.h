#include "generos.h"

typedef struct {
    int id;
    char *titulo;
    Genero *generos;
    char *diretor;
    int ano;
    // imagem capa;
} Filme;

Filme *criar_filme(char *titulo, char *genero, char *diretor, int ano);
void adiciona_genero(Filme *filme, char *genero);
void deletar_filme(Filme *filme);
void print(Filme *filme);
void print_titulo_id(Filme *filme);
