#include <cstdio>
#include <cstring>
#include <cstdlib>

#define MAX_TAM 100

struct Livro {
    char titulo[MAX_TAM];
    char autor[MAX_TAM];
    int codigo;
    int ano;
    float preco;
    int emprestado;
};

void limpaBuffer();
long tamanho(FILE *fp);
void cadastrar(FILE *fp);
void consultar(FILE *fp);
void listarTodos(FILE *fp);
void emprestar(FILE *fp);
void devolver(FILE *fp);
void mostrarLivro(const Livro &l, long pos);
void ler_string_segura(char *buffer, int tamanho);

void limpaBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

long tamanho(FILE *fp) {
    if (!fp) return 0;
    fseek(fp, 0, SEEK_END);
    long bytes = ftell(fp);
    return bytes / (long)sizeof(Livro);
}

void ler_string_segura(char *buffer, int tamanho) {
    if (fgets(buffer, tamanho, stdin) != NULL)
        buffer[strcspn(buffer, "\n")] = '\0';
    else
        buffer[0] = '\0';
}

void cadastrar(FILE *fp) {
    Livro l;
    printf("Título: ");
    ler_string_segura(l.titulo, MAX_TAM);

    printf("Autor: ");
    ler_string_segura(l.autor, MAX_TAM);

    printf("Código: ");
    if (scanf("%d", &l.codigo) != 1) { limpaBuffer(); return; }
    limpaBuffer();

    printf("Ano: ");
    if (scanf("%d", &l.ano) != 1) { limpaBuffer(); return; }
    limpaBuffer();

    printf("Preço: ");
    if (scanf("%f", &l.preco) != 1) { limpaBuffer(); return; }
    limpaBuffer();

    l.emprestado = 0;

    fseek(fp, 0, SEEK_END);
    fwrite(&l, sizeof(Livro), 1, fp);
    fflush(fp);

    printf("Livro cadastrado na posição: %ld\n", tamanho(fp) - 1);
}

void consultar(FILE *fp) {
    long n = tamanho(fp);
    if (n == 0) { printf("Sem registros.\n"); return; }

    long idx;
    printf("Índice (0 a %ld): ", n - 1);
    if (scanf("%ld", &idx) != 1) { limpaBuffer(); return; }
    limpaBuffer();

    if (idx < 0 || idx >= n) { printf("Índice inválido.\n"); return; }

    fseek(fp, idx * sizeof(Livro), SEEK_SET);
    Livro l;
    fread(&l, sizeof(Livro), 1, fp);

    mostrarLivro(l, idx);
}

void mostrarLivro(const Livro &l, long pos) {
    printf("---- Registro %ld ----\n", pos);
    printf("Título: %s\n", l.titulo);
    printf("Autor: %s\n", l.autor);
    printf("Código: %d\n", l.codigo);
    printf("Ano: %d\n", l.ano);
    printf("Preço: %.2f\n", l.preco);
    printf("Status: %s\n", l.emprestado ? "Emprestado" : "Disponível");
}

void listarTodos(FILE *fp) {
    long n = tamanho(fp);
    if (n == 0) { printf("Sem registros.\n"); return; }

    for (long i = 0; i < n; i++) {
        fseek(fp, i * sizeof(Livro), SEEK_SET);
        Livro l;
        fread(&l, sizeof(Livro), 1, fp);
        printf("[%ld] %s - %s (%s)\n", i, l.titulo, l.autor, l.emprestado ? "Emprestado" : "Disponível");
    }
}

void emprestar(FILE *fp) {
    long n = tamanho(fp);
    if (n == 0) { printf("Sem registros.\n"); return; }

    long idx;
    printf("Índice para emprestar (0 a %ld): ", n - 1);
    if (scanf("%ld", &idx) != 1) { limpaBuffer(); return; }
    limpaBuffer();

    if (idx < 0 || idx >= n) { printf("Índice inválido.\n"); return; }

    fseek(fp, idx * sizeof(Livro), SEEK_SET);
    Livro l;
    fread(&l, sizeof(Livro), 1, fp);

    if (l.emprestado) { printf("Já emprestado.\n"); mostrarLivro(l, idx); return; }

    l.emprestado = 1;

    fseek(fp, idx * sizeof(Livro), SEEK_SET);
    fwrite(&l, sizeof(Livro), 1, fp);
    fflush(fp);

    printf("Empréstimo registrado.\n");
    mostrarLivro(l, idx);
}

void devolver(FILE *fp) {
    long n = tamanho(fp);
    if (n == 0) { printf("Sem registros.\n"); return; }

    long idx;
    printf("Índice para devolver (0 a %ld): ", n - 1);
    if (scanf("%ld", &idx) != 1) { limpaBuffer(); return; }
    limpaBuffer();

    if (idx < 0 || idx >= n) { printf("Índice inválido.\n"); return; }

    fseek(fp, idx * sizeof(Livro), SEEK_SET);
    Livro l;
    fread(&l, sizeof(Livro), 1, fp);

    if (!l.emprestado) { printf("Livro já está disponível.\n"); mostrarLivro(l, idx); return; }

    l.emprestado = 0;

    fseek(fp, idx * sizeof(Livro), SEEK_SET);
    fwrite(&l, sizeof(Livro), 1, fp);
    fflush(fp);

    printf("Devolução registrada.\n");
    mostrarLivro(l, idx);
}

int main() {
    FILE *fp = fopen("biblioteca.dat", "r+b");
    if (!fp) fp = fopen("biblioteca.dat", "w+b");
    if (!fp) { printf("Erro ao abrir arquivo.\n"); return 1; }

    int opcao;
    do {
        printf("\nMenu\n");
        printf("1 - Cadastrar livro\n");
        printf("2 - Consultar livro\n");
        printf("3 - Listar todos\n");
        printf("4 - Emprestar livro\n");
        printf("5 - Devolver livro\n");
        printf("6 - Quantidade de registros\n");
        printf("0 - Sair\nEscolha: ");

        if (scanf("%d", &opcao) != 1) { limpaBuffer(); continue; }
        limpaBuffer();

        switch (opcao) {
            case 1: cadastrar(fp); break;
            case 2: consultar(fp); break;
            case 3: listarTodos(fp); break;
            case 4: emprestar(fp); break;
            case 5: devolver(fp); break;
            case 6: printf("Registros: %ld\n", tamanho(fp)); break;
            case 0: break;
            default: printf("Opção inválida.\n");
        }
    } while (opcao != 0);

    fclose(fp);
    return 0;
}
