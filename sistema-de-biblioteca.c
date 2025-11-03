/*
  Sistema de Biblioteca – Nível Mestre (C, arquivo único)
  Autor: (coloque seu nome)
  Objetivo:
    - Demonstrar modularização, ponteiros, passagem por valor (exibição) e por referência (atualizações),
      e alocação dinâmica (malloc/calloc/realloc/free) em um sistema simples de biblioteca.
    - Estruturas: Livro, Usuario, Emprestimo.
    - Operações: cadastrar livros/usuários, emprestar, devolver, listar e buscar.

  Observações:
    - Usei "passagem por valor" nas funções de exibição (ex.: exibirLivro(Livro l)).
    - Usei "passagem por referência" nas funções que atualizam estado (ex.: realizarEmprestimo(...)).
    - Todos os vetores são dinâmicos com crescimento por realocação.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ---------------------- Constantes ---------------------- */
#define TITULO_MAX 80
#define AUTOR_MAX  60
#define NOME_MAX   60

/* ---------------------- Structs ---------------------- */
typedef struct {
    int id;
    char titulo[TITULO_MAX];
    char autor[AUTOR_MAX];
    int ano;
    int exemplares;   /* total cadastrados */
    int disponiveis;  /* quantos ainda podem ser emprestados */
} Livro;

typedef struct {
    int id;
    char nome[NOME_MAX];
} Usuario;

typedef struct {
    int id;
    int idLivro;
    int idUsuario;
    time_t data; /* data do empréstimo */
    int ativo;   /* 1 = empréstimo em aberto, 0 = devolvido */
} Emprestimo;

/* ---------------------- Protótipos ---------------------- */
/* Inicialização e memória */
void inicializar(Livro** livros, int* nLiv, int* capLiv,
                 Usuario** usuarios, int* nUsu, int* capUsu,
                 Emprestimo** emps, int* nEmp, int* capEmp);
void liberarMemoria(Livro* livros, Usuario* usuarios, Emprestimo* emps);

/* Helpers de ID/Busca */
int proximoIdLivro(const Livro* v, int n);
int proximoIdUsuario(const Usuario* v, int n);
int proximoIdEmprestimo(const Emprestimo* v, int n);
int buscarLivroPorId(const Livro* v, int n, int id);
int buscarUsuarioPorId(const Usuario* v, int n, int id);
int buscarEmprestimoAtivo(const Emprestimo* v, int n, int idEmp);

/* Cadastro (atualizam por referência) */
int adicionarLivro(Livro** v, int* n, int* cap, Livro novo);
int adicionarUsuario(Usuario** v, int* n, int* cap, Usuario novo);

/* Empréstimo e Devolução (atualizam por referência) */
int realizarEmprestimo(Livro* livros, int nLiv,
                       Usuario* usuarios, int nUsu,
                       Emprestimo** emps, int* nEmp, int* capEmp,
                       int idLivro, int idUsuario);
int devolverEmprestimo(Livro* livros, int nLiv,
                       Emprestimo* emps, int nEmp,
                       int idEmprestimo);

/* Exibição (passagem por valor) */
void exibirLivro(Livro l);
void exibirUsuario(Usuario u);
void exibirEmprestimo(const Emprestimo e, const Livro* livros, int nLiv, const Usuario* usuarios, int nUsu);
void listarLivros(const Livro* v, int n);
void listarUsuarios(const Usuario* v, int n);
void listarEmprestimos(const Emprestimo* v, int n, const Livro* livros, int nLiv, const Usuario* usuarios, int nUsu);

/* Utilitários */
void limparBufferEntrada(void);
void titulo(const char* s);
void linha(void);
void formatarData(time_t t, char* buf, size_t tam);

/* ---------------------- Implementações ---------------------- */

void inicializar(Livro** livros, int* nLiv, int* capLiv,
                 Usuario** usuarios, int* nUsu, int* capUsu,
                 Emprestimo** emps, int* nEmp, int* capEmp) {
    *nLiv = *nUsu = *nEmp = 0;
    *capLiv = *capUsu = *capEmp = 4;

    *livros = (Livro*)calloc(*capLiv, sizeof(Livro));
    *usuarios = (Usuario*)calloc(*capUsu, sizeof(Usuario));
    *emps = (Emprestimo*)calloc(*capEmp, sizeof(Emprestimo));

    if (!*livros || !*usuarios || !*emps) {
        fprintf(stderr, "Falha ao alocar memória inicial.\n");
        exit(EXIT_FAILURE);
    }
}

void liberarMemoria(Livro* livros, Usuario* usuarios, Emprestimo* emps) {
    free(livros);
    free(usuarios);
    free(emps);
}

int proximoIdLivro(const Livro* v, int n) {
    int max = 0;
    for (int i = 0; i < n; i++) if (v[i].id > max) max = v[i].id;
    return max + 1;
}
int proximoIdUsuario(const Usuario* v, int n) {
    int max = 0;
    for (int i = 0; i < n; i++) if (v[i].id > max) max = v[i].id;
    return max + 1;
}
int proximoIdEmprestimo(const Emprestimo* v, int n) {
    int max = 0;
    for (int i = 0; i < n; i++) if (v[i].id > max) max = v[i].id;
    return max + 1;
}

int buscarLivroPorId(const Livro* v, int n, int id) {
    for (int i = 0; i < n; i++) if (v[i].id == id) return i;
    return -1;
}
int buscarUsuarioPorId(const Usuario* v, int n, int id) {
    for (int i = 0; i < n; i++) if (v[i].id == id) return i;
    return -1;
}
int buscarEmprestimoAtivo(const Emprestimo* v, int n, int idEmp) {
    for (int i = 0; i < n; i++) if (v[i].id == idEmp && v[i].ativo) return i;
    return -1;
}

int adicionarLivro(Livro** v, int* n, int* cap, Livro novo) {
    if (*n >= *cap) {
        int novoCap = (*cap) * 2;
        Livro* temp = (Livro*)realloc(*v, novoCap * sizeof(Livro));
        if (!temp) return 0;
        *v = temp;
        *cap = novoCap;
    }
    (*v)[*n] = novo;
    (*n)++;
    return 1;
}

int adicionarUsuario(Usuario** v, int* n, int* cap, Usuario novo) {
    if (*n >= *cap) {
        int novoCap = (*cap) * 2;
        Usuario* temp = (Usuario*)realloc(*v, novoCap * sizeof(Usuario));
        if (!temp) return 0;
        *v = temp;
        *cap = novoCap;
    }
    (*v)[*n] = novo;
    (*n)++;
    return 1;
}

int realizarEmprestimo(Livro* livros, int nLiv,
                       Usuario* usuarios, int nUsu,
                       Emprestimo** emps, int* nEmp, int* capEmp,
                       int idLivro, int idUsuario) {
    int idxL = buscarLivroPorId(livros, nLiv, idLivro);
    int idxU = buscarUsuarioPorId(usuarios, nUsu, idUsuario);

    if (idxL < 0 || idxU < 0) {
        puts("Livro ou usuário inexistente.");
        return 0;
    }
    if (livros[idxL].disponiveis <= 0) {
        puts("Sem exemplares disponíveis para este livro.");
        return 0;
    }

    /* Realoca se necessário */
    if (*nEmp >= *capEmp) {
        int novoCap = (*capEmp) * 2;
        Emprestimo* temp = (Emprestimo*)realloc(*emps, novoCap * sizeof(Emprestimo));
        if (!temp) return 0;
        *emps = temp;
        *capEmp = novoCap;
    }

    Emprestimo e;
    e.id = proximoIdEmprestimo(*emps, *nEmp);
    e.idLivro = idLivro;
    e.idUsuario = idUsuario;
    e.data = time(NULL);
    e.ativo = 1;

    (*emps)[*nEmp] = e;
    (*nEmp)++;

    /* Atualização por referência: reduz disponíveis do livro */
    livros[idxL].disponiveis--;

    puts("Empréstimo registrado com sucesso.");
    return 1;
}

int devolverEmprestimo(Livro* livros, int nLiv,
                       Emprestimo* emps, int nEmp,
                       int idEmprestimo) {
    int idxE = buscarEmprestimoAtivo(emps, nEmp, idEmprestimo);
    if (idxE < 0) {
        puts("Empréstimo não encontrado ou já devolvido.");
        return 0;
    }
    int idxL = buscarLivroPorId(livros, nLiv, emps[idxE].idLivro);
    if (idxL < 0) {
        puts("Livro do empréstimo não existe mais no acervo.");
        return 0;
    }
    /* Atualização por referência: marcar devolução e devolver exemplar */
    emps[idxE].ativo = 0;
    livros[idxL].disponiveis++;
    puts("Devolução realizada com sucesso.");
    return 1;
}

/* ---------------------- Exibições (valor) ---------------------- */
void exibirLivro(Livro l) {
    printf("#%d | \"%s\" (%d) - %s | ex: %d, disp: %d\n",
           l.id, l.titulo, l.ano, l.autor, l.exemplares, l.disponiveis);
}

void exibirUsuario(Usuario u) {
    printf("#%d | %s\n", u.id, u.nome);
}

void formatarData(time_t t, char* buf, size_t tam) {
    struct tm* tm_info = localtime(&t);
    strftime(buf, tam, "%d/%m/%Y %H:%M", tm_info);
}

void exibirEmprestimo(const Emprestimo e, const Livro* livros, int nLiv, const Usuario* usuarios, int nUsu) {
    const char* status = e.ativo ? "ABERTO" : "FECHADO";
    int iL = buscarLivroPorId(livros, nLiv, e.idLivro);
    int iU = buscarUsuarioPorId(usuarios, nUsu, e.idUsuario);
    char data[20] = {0};
    formatarData(e.data, data, sizeof(data));
    printf("#%d | Livro: %s (ID %d) | Usuário: %s (ID %d) | %s | %s\n",
           e.id,
           (iL >= 0 ? livros[iL].titulo : "<removido>"), e.idLivro,
           (iU >= 0 ? usuarios[iU].nome  : "<removido>"), e.idUsuario,
           data, status);
}

void listarLivros(const Livro* v, int n) {
    titulo("LIVROS");
    if (n == 0) { puts("(vazio)"); return; }
    for (int i = 0; i < n; i++) exibirLivro(v[i]);
}

void listarUsuarios(const Usuario* v, int n) {
    titulo("USUÁRIOS");
    if (n == 0) { puts("(vazio)"); return; }
    for (int i = 0; i < n; i++) exibirUsuario(v[i]);
}

void listarEmprestimos(const Emprestimo* v, int n, const Livro* livros, int nLiv, const Usuario* usuarios, int nUsu) {
    titulo("EMPRÉSTIMOS");
    if (n == 0) { puts("(vazio)"); return; }
    for (int i = 0; i < n; i++) exibirEmprestimo(v[i], livros, nLiv, usuarios, nUsu);
}

/* ---------------------- Utilitários ---------------------- */
void limparBufferEntrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { /* descarta */ }
}
void titulo(const char* s) {
    linha();
    printf("%s\n", s);
    linha();
}
void linha(void) {
    puts("------------------------------------------------------------");
}

/* ---------------------- Main / Menu ---------------------- */
int main(void) {
    Livro* livros = NULL;     int nLiv=0, capLiv=0;
    Usuario* usuarios = NULL; int nUsu=0, capUsu=0;
    Emprestimo* emps = NULL;  int nEmp=0, capEmp=0;

    inicializar(&livros, &nLiv, &capLiv,
                &usuarios, &nUsu, &capUsu,
                &emps, &nEmp, &capEmp);

    /* Cadastro inicial opcional para facilitar testes */
    Livro l1 = { .id = 1, .titulo = "Algoritmos", .autor = "Cormen", .ano = 2009, .exemplares = 3, .disponiveis = 3 };
    Livro l2 = { .id = 2, .titulo = "C em Acao", .autor = "K&R", .ano = 1988, .exemplares = 2, .disponiveis = 2 };
    adicionarLivro(&livros, &nLiv, &capLiv, l1);
    adicionarLivro(&livros, &nLiv, &capLiv, l2);

    Usuario u1 = { .id = 1, .nome = "Ana Silva" };
    Usuario u2 = { .id = 2, .nome = "Bruno Costa" };
    adicionarUsuario(&usuarios, &nUsu, &capUsu, u1);
    adicionarUsuario(&usuarios, &nUsu, &capUsu, u2);

    int opc = -1;
    do {
        titulo("SISTEMA DE BIBLIOTECA");
        puts("1 - Listar livros");
        puts("2 - Cadastrar livro");
        puts("3 - Listar usuarios");
        puts("4 - Cadastrar usuario");
        puts("5 - Registrar emprestimo");
        puts("6 - Registrar devolucao");
        puts("7 - Listar emprestimos");
        puts("0 - Sair");
        linha();
        printf("Opcao: ");
        if (scanf("%d", &opc) != 1) { puts("Entrada invalida."); break; }
        limparBufferEntrada();

        if (opc == 1) {
            listarLivros(livros, nLiv);
        } else if (opc == 2) {
            Livro novo;
            /* Passagem por referência ao inserir, por valor ao exibir */
            novo.id = proximoIdLivro(livros, nLiv);
            printf("Titulo: "); fgets(novo.titulo, TITULO_MAX, stdin);
            novo.titulo[strcspn(novo.titulo, "\n")] = 0;
            printf("Autor: "); fgets(novo.autor, AUTOR_MAX, stdin);
            novo.autor[strcspn(novo.autor, "\n")] = 0;
            printf("Ano: "); scanf("%d", &novo.ano); limparBufferEntrada();
            printf("Exemplares: "); scanf("%d", &novo.exemplares); limparBufferEntrada();
            if (novo.exemplares < 0) novo.exemplares = 0;
            novo.disponiveis = novo.exemplares;

            if (adicionarLivro(&livros, &nLiv, &capLiv, novo)) {
                puts("Livro cadastrado:");
                exibirLivro(novo); /* por valor */
            } else {
                puts("Falha ao cadastrar livro.");
            }

        } else if (opc == 3) {
            listarUsuarios(usuarios, nUsu);

        } else if (opc == 4) {
            Usuario novo;
            novo.id = proximoIdUsuario(usuarios, nUsu);
            printf("Nome: "); fgets(novo.nome, NOME_MAX, stdin);
            novo.nome[strcspn(novo.nome, "\n")] = 0;

            if (adicionarUsuario(&usuarios, &nUsu, &capUsu, novo)) {
                puts("Usuario cadastrado:");
                exibirUsuario(novo); /* por valor */
            } else {
                puts("Falha ao cadastrar usuario.");
            }

        } else if (opc == 5) {
            int idL, idU;
            listarLivros(livros, nLiv);
            printf("ID do livro para emprestar: "); scanf("%d", &idL); limparBufferEntrada();

            listarUsuarios(usuarios, nUsu);
            printf("ID do usuario: "); scanf("%d", &idU); limparBufferEntrada();

            if (!realizarEmprestimo(livros, nLiv, usuarios, nUsu,
                                    &emps, &nEmp, &capEmp, idL, idU)) {
                puts("Nao foi possivel registrar o emprestimo.");
            }

        } else if (opc == 6) {
            int idE;
            listarEmprestimos(emps, nEmp, livros, nLiv, usuarios, nUsu);
            printf("ID do emprestimo a devolver: "); scanf("%d", &idE); limparBufferEntrada();
            if (!devolverEmprestimo(livros, nLiv, emps, nEmp, idE)) {
                puts("Nao foi possivel registrar a devolucao.");
            }

        } else if (opc == 7) {
            listarEmprestimos(emps, nEmp, livros, nLiv, usuarios, nUsu);

        } else if (opc == 0) {
            puts("Encerrando...");

        } else {
            puts("Opcao invalida.");
        }

    } while (opc != 0);

    liberarMemoria(livros, usuarios, emps);
    return 0;
}
