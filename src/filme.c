#include "filme.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_MAX_GENEROS 256
// NÙMERO MÀXIMO SUPORTADO DE ID È 999
int qtd_filmes = 0;

Genero *novo_genero() {
    Genero *novo = calloc(1, sizeof(Genero));
    novo->genero = calloc(NUM_MAX_GENEROS, sizeof(char));
    novo->qtd = 0;
    return novo;
}

void adiciona_genero(Filme *filme, char *g) {
    if (filme->generos->qtd < NUM_MAX_GENEROS) {
        filme->generos->genero[filme->generos->qtd] = g;
        filme->generos->qtd++;
    } else {
        printf("Não é possível adicionar mais gêneros a esse filme");
    }
}

Filme *create_filme(char *titulo, char *genero, char *diretor, int ano) {
    Filme *novo = calloc(1, sizeof(Filme));
    novo->id = qtd_filmes++;
    novo->titulo = titulo;
    novo->generos = novo_genero();
    adiciona_genero(novo, genero);
    novo->diretor = diretor;
    novo->ano = ano;
    return novo;
}
void print(Filme *filme) {
    printf("%d %s %s %d\n", filme->id, filme->titulo, filme->diretor,
           filme->ano);
    // TODO print generos!
}
void print_titulo_id(Filme *filme) {
    printf("%d %s\n", filme->id, filme->titulo);
}
void delete_filme(Filme *filme) {
    free(filme->generos->genero);
    free(filme->generos);
    free(filme->titulo);
    free(filme->diretor);
    free(filme);
}
