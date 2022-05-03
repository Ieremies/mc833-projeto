#include "filme.h"
#include <stdio.h>
#include <stdlib.h>

// NÙMERO MÀXIMO SUPORTADO DE ID È 999
int qtd_filmes = 0;

Filme *criar_filme(char *titulo, char *genero, char *diretor, int ano) {
    Filme *novo = calloc(1, sizeof(Filme));
    novo->id = qtd_filmes++;
    novo->titulo = titulo;
    novo->generos = criar_generos();
    adiciona_genero(novo, genero);
    novo->diretor = diretor;
    novo->ano = ano;
    return novo;
}

void adiciona_genero(Filme *filme, char *novo_genero) {
    adiciona_generos(filme->generos, novo_genero);
}

void print_filme(Filme *filme) {
    printf("%d %s %s %d\n", filme->id, filme->titulo, filme->diretor,
           filme->ano);
    // TODO print generos!
}

void print_titulo_id(Filme *filme) {
    printf("%d %s\n", filme->id, filme->titulo);
}

void deletar_filme(Filme *filme) {
    deleta_generos(filme->generos);
    free(filme->titulo);
    free(filme->diretor);
    free(filme);
}
