#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>
#include <ctype.h>

// =================== STRUCT ===================
typedef struct {
    char nome[100];
    char setor;
    int idade;
} Funcionario;

// ================ PROTÓTIPOS ==================
void criarArquivoCSV();
void troca(Funcionario *a, Funcionario *b);
void sortNome(Funcionario *v, int size);
void sortSetor(Funcionario *v, int size);
void sortIdade(Funcionario *v, int size);
Funcionario* lerArquivo(const char *nomeArquivo, int *qtdDeFuncionarios);
void ordenar(Funcionario *v, int qtdDeFuncionarios, int opcao);
void imprimirFuncionarios(Funcionario *v, int qtdDeFuncionarios);
void salvarArquivoNovo(Funcionario *v, int qtdDeFuncionarios);
void trim(char *str);
void flushEntrada();

// =================== MAIN =====================
int main() {
    // Configurando a localidade para suportar acentos
    setlocale(LC_ALL, "pt_BR.UTF-8");

    int qtdDeFuncionarios;
    int opcao;
    int valido = 0;

    criarArquivoCSV();

    Funcionario *funcionarios = lerArquivo("Funcionarios.csv", &qtdDeFuncionarios);
    if (funcionarios == NULL) return 1;

    if (qtdDeFuncionarios == 0) {
        printf("\nO arquivo não contém registros válidos.\n");
        free(funcionarios);
        return 1;
    }

    printf("\nDeseja ordenar por:\n");
    printf("1 - Nome\n2 - Setor\n3 - Idade\n");

    do {
        printf("Digite a opção desejada: ");
        if (scanf("%d", &opcao) != 1) {
            printf("Entrada inválida. Por favor, digite um número.\n");
            flushEntrada();
            continue;
        }
        if (opcao >= 1 && opcao <= 3) {
            valido = 1;
        } else {
            printf("Opção inválida. Tente novamente.\n");
        }
    } while (!valido);

    ordenar(funcionarios, qtdDeFuncionarios, opcao);
    imprimirFuncionarios(funcionarios, qtdDeFuncionarios);
    salvarArquivoNovo(funcionarios, qtdDeFuncionarios);

    free(funcionarios);
    return 0;
}

// ============ CRIA ARQUIVO CSV =============
void criarArquivoCSV() {
    FILE *f = fopen("Funcionarios.csv", "w");
    if (f == NULL) {
        printf("Erro ao criar o arquivo Funcionarios.csv.\n");
        return;
    }

    // Dados de exemplo com acentos
    fprintf(f, "João da Silva, A ,32\n");
    fprintf(f, "Maria Oliveira,B,45\n");
    fprintf(f, "Carlos Souza, C ,28\n");
    fprintf(f, "Ana Lima,A,38\n");
    fprintf(f, "Pedro Santos,B,30\n");
    fprintf(f, "Lucas Rocha,C,29\n");
    fprintf(f, "Fernanda Melo,A,42\n");
    fprintf(f, "Rafael Duarte,B,35\n");

    fclose(f);
    printf("Arquivo 'Funcionarios.csv' criado automaticamente.\n");
}

// ============ LÊ O CSV =============
Funcionario* lerArquivo(const char *nomeArquivo, int *qtdDeFuncionarios) {
    FILE *entrada = fopen(nomeArquivo, "r");
    if (!entrada) {
        perror("Erro ao abrir o arquivo");
        return NULL;
    }

    int capacidade = 10;
    Funcionario *funcionarios = malloc(capacidade * sizeof(Funcionario));
    if (!funcionarios) {
        perror("Erro ao alocar memória");
        fclose(entrada);
        return NULL;
    }

    char buffer[300];
    int registros = 0;
    int linha = 0;

    while (fgets(buffer, sizeof(buffer), entrada)) {
        linha++;
        if (buffer[0] == '\n' || buffer[0] == '\0')
            continue;

        if (registros >= capacidade) {
            capacidade *= 2;
            Funcionario *temp = realloc(funcionarios, capacidade * sizeof(Funcionario));
            if (!temp) {
                perror("Erro ao realocar memória");
                free(funcionarios);
                fclose(entrada);
                return NULL;
            }
            funcionarios = temp;
        }

        Funcionario f;
        if (sscanf(buffer, " %99[^,], %c ,%d", f.nome, &f.setor, &f.idade) != 3) {
            fprintf(stderr, "Formato inválido na linha %d: %s", linha, buffer);
            continue;
        }

        trim(f.nome);
        funcionarios[registros++] = f;
    }

    fclose(entrada);
    *qtdDeFuncionarios = registros;
    return funcionarios;
}

// ============ ORDENAÇÃO =============
void ordenar(Funcionario *v, int qtd, int opcao) {
    switch (opcao) {
        case 1: sortNome(v, qtd); break;
        case 2: sortSetor(v, qtd); break;
        case 3: sortIdade(v, qtd); break;
        default:
            printf("Opção inválida.\n");
            exit(1);
    }
}

void sortNome(Funcionario *v, int size) {
    for (int i = 0; i < size - 1; i++) {
        if (strcmp(v[i].nome, v[i + 1].nome) > 0) {
            troca(&v[i], &v[i + 1]);
            i = -1;
        }
    }
}

void sortSetor(Funcionario *v, int size) {
    for (int i = 0; i < size - 1; i++) {
        if (v[i].setor > v[i + 1].setor) {
            troca(&v[i], &v[i + 1]);
            i = -1;
        }
    }
}

void sortIdade(Funcionario *v, int size) {
    for (int i = 0; i < size - 1; i++) {
        if (v[i].idade > v[i + 1].idade) {
            troca(&v[i], &v[i + 1]);
            i = -1;
        }
    }
}

void troca(Funcionario *a, Funcionario *b) {
    Funcionario temp = *a;
    *a = *b;
    *b = temp;
}

// ============ IMPRIME FUNCIONÁRIOS ============
void imprimirFuncionarios(Funcionario *v, int qtd) {
    printf("\n=========================================================\n");
    printf("%d registros válidos lidos e ordenados:\n", qtd);
    for (int i = 0; i < qtd; i++) {
        printf("-----------------------------------------\n");
        printf("%s | Setor: %c | Idade: %d\n", v[i].nome, v[i].setor, v[i].idade);
    }
    printf("=========================================================\n");
}

// ============ SALVA NOVO CSV ============
void salvarArquivoNovo(Funcionario *v, int qtd) {
    char arquivoNovo[100];

    printf("\nDigite o nome do novo arquivo CSV para os dados ordenados: ");
    flushEntrada();
    fgets(arquivoNovo, sizeof(arquivoNovo), stdin);
    arquivoNovo[strcspn(arquivoNovo, "\n")] = 0;

    FILE *saida = fopen(arquivoNovo, "w");
    if (!saida) {
        perror("Erro ao criar o arquivo de saída");
        return;
    }

    for (int i = 0; i < qtd; i++) {
        fprintf(saida, "%s,%c,%d\n", v[i].nome, v[i].setor, v[i].idade);
    }

    fclose(saida);
    printf("Arquivo '%s' gerado com sucesso!\n", arquivoNovo);
}

// ============ FUNÇÕES UTILITÁRIAS ============

// Remove espaços
void trim(char *str) {
    char *inicio = str;
    while (isspace((unsigned char)*inicio)) inicio++;

    char *fim = inicio + strlen(inicio) - 1;
    while (fim > inicio && isspace((unsigned char)*fim)) fim--;
    *(fim + 1) = '\0';

    memmove(str, inicio, strlen(inicio) + 1);
}

// Limpa o buffer do teclado
void flushEntrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
