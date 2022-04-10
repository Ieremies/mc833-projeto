
#define NUM_MAX_GENEROS 256

typedef struct {
    char **genero;
    int qtd;
} Genero;

Genero *criar_generos();
void adiciona_generos(Genero *lista_generos, char *genero);
void deletar_generos(Genero *lista_generos);
