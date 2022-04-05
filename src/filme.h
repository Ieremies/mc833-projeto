

typedef struct {
    char **genero;
    int qtd;
} Genero;

typedef struct {
    int id;
    char *titulo;
    Genero *generos;
    char *diretor;
    int ano;
    // imagem capa;
} Filme;

Filme *create_filme(char *titulo, char *genero, char *diretor, int ano);
void adiciona_genero(Filme *filme, char *genero);
void print(Filme *filme);
void print_titulo_id(Filme *filme);
void delete_filme(Filme *filme);
