#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#define PLATE_LEN 8
#define TAM_LINHA 64
#define REPETICOES 100

typedef enum {
    EMPTY = 0,
    OCCUPIED = 1,
    DELETED = 2
} Status;

typedef struct {
    char placa[PLATE_LEN];
    Status status;
} HashEntry;

int ehPrimo(size_t numero) {
    if (numero < 2) return 0;
    for (size_t divisor = 2; divisor <= numero / divisor; divisor++) {
        if (numero % divisor == 0) return 0;
    }
    return 1;
}

size_t calcularTamanhoTabela(size_t n) {
    size_t minimo = (6 * n + 4) / 5;
    size_t tamanho = minimo + 1;
    while (!ehPrimo(tamanho)) tamanho++;
    return tamanho;
}

size_t hash1(const char *placa, size_t tamanho) {
    uint64_t hash = 5381;
    while (*placa != '\0') {
        hash = hash * 33 + (unsigned char)*placa;
        placa++;
    }
    return (size_t)(hash % tamanho);
}

size_t hash2(const char *placa, size_t tamanho) {
    uint64_t hash = 0;
    while (*placa != '\0') {
        hash = hash * 37 + (unsigned char)*placa;
        placa++;
    }
    return 1 + (size_t)(hash % (tamanho - 2));
}

void inicializarTabela(HashEntry tabela[], size_t tamanho) {
    for (size_t i = 0; i < tamanho; i++) {
        tabela[i].status = EMPTY;
        tabela[i].placa[0] = '\0';
    }
}

int inserir(HashEntry tabela[], size_t tamanho, const char *placa,
            unsigned long long *total_colisoes) {
    size_t inicio = hash1(placa, tamanho);
    size_t passo = hash2(placa, tamanho);
    size_t primeira_excluida = tamanho;
    unsigned long long colisoes_locais = 0;

    for (size_t i = 0; i < tamanho; i++) {
        size_t idx = (inicio + i * passo) % tamanho;

        if (tabela[idx].status == OCCUPIED) {
            if (strcmp(tabela[idx].placa, placa) == 0) {
                return 0;
            }
            colisoes_locais++;
        } else if (tabela[idx].status == DELETED) {
            if (primeira_excluida == tamanho) primeira_excluida = idx;
        } else {
            size_t destino = (primeira_excluida != tamanho)
                           ? primeira_excluida : idx;
            strcpy(tabela[destino].placa, placa);
            tabela[destino].status = OCCUPIED;
            *total_colisoes += colisoes_locais;
            return 1;
        }
    }

    if (primeira_excluida != tamanho) {
        strcpy(tabela[primeira_excluida].placa, placa);
        tabela[primeira_excluida].status = OCCUPIED;
        *total_colisoes += colisoes_locais;
        return 1;
    }
    return -1;
}

int buscar(const HashEntry tabela[], size_t tamanho, const char *placa) {
    size_t inicio = hash1(placa, tamanho);
    size_t passo = hash2(placa, tamanho);

    for (size_t i = 0; i < tamanho; i++) {
        size_t idx = (inicio + i * passo) % tamanho;

        if (tabela[idx].status == EMPTY) return -1;
        if (tabela[idx].status == OCCUPIED &&
            strcmp(tabela[idx].placa, placa) == 0) {
            return (int)idx;
        }
    }
    return -1;
}

int excluir(HashEntry tabela[], size_t tamanho, const char *placa) {
    int idx = buscar(tabela, tamanho, placa);
    if (idx < 0) return 0;
    tabela[idx].status = DELETED;
    return 1;
}

int placaValida(const char *placa) {
    if (strlen(placa) != 7) return 0;
    for (int i = 0; i < 3; i++) {
        if (placa[i] < 'A' || placa[i] > 'Z') return 0;
    }
    for (int i = 3; i < 7; i++) {
        if (placa[i] < '0' || placa[i] > '9') return 0;
    }
    return 1;
}

int lerPlaca(FILE *arquivo, char placa[PLATE_LEN]) {
    char linha[TAM_LINHA];
    if (fgets(linha, sizeof(linha), arquivo) == NULL) return 0;

    size_t comprimento = strlen(linha);
    if (comprimento == TAM_LINHA - 1 && linha[comprimento - 1] != '\n') {
        int ch;
        while ((ch = fgetc(arquivo)) != EOF && ch != '\n') { }
        return -1;
    }

    while (comprimento > 0 &&
           (linha[comprimento - 1] == '\n' || linha[comprimento - 1] == '\r')) {
        linha[--comprimento] = '\0';
    }

    if (!placaValida(linha)) return -1;
    memcpy(placa, linha, PLATE_LEN);
    return 1;
}

double diferencaNs(struct timespec inicio, struct timespec fim) {
    return (double)(fim.tv_sec - inicio.tv_sec) * 1000000000.0
         + (double)(fim.tv_nsec - inicio.tv_nsec);
}

void marcarTempo(struct timespec *instante) {
    if (timespec_get(instante, TIME_UTC) != TIME_UTC) {
        fprintf(stderr, "Erro ao obter relogio de alta resolucao.\n");
        exit(EXIT_FAILURE);
    }
}

int executarTeste(FILE *arquivo, FILE *csv, int quantidade) {
    size_t tamanho = calcularTamanhoTabela((size_t)quantidade);
    HashEntry *tabela = malloc(tamanho * sizeof(*tabela));
    if (tabela == NULL) {
        fprintf(stderr, "Falha de memoria para tabela com %zu posicoes.\n", tamanho);
        return 0;
    }

    double ns_insercao = 0.0, ns_busca = 0.0, ns_exclusao = 0.0;
    unsigned long long colisoes_relatorio = 0;
    int inseridas = 0, encontradas = 0, excluidas = 0;
    int sucesso = 1;
    char placa[PLATE_LEN];
    struct timespec inicio, fim;

    for (int repeticao = 0; repeticao < REPETICOES && sucesso; repeticao++) {
        unsigned long long colisoes = 0;
        int insercoes_atual = 0, buscas_atual = 0, exclusoes_atual = 0;
        inicializarTabela(tabela, tamanho);

        rewind(arquivo);
        for (int i = 0; i < quantidade; i++) {
            if (lerPlaca(arquivo, placa) != 1) {
                fprintf(stderr, "Entrada insuficiente/invalida na linha %d.\n", i + 1);
                sucesso = 0;
                break;
            }
            marcarTempo(&inicio);
            int resultado = inserir(tabela, tamanho, placa, &colisoes);
            marcarTempo(&fim);
            ns_insercao += diferencaNs(inicio, fim);
            if (resultado != 1) {
                fprintf(stderr, "Nao foi possivel inserir %s (resultado %d).\n",
                        placa, resultado);
                sucesso = 0;
                break;
            }
            insercoes_atual++;
        }
        if (!sucesso) break;

        rewind(arquivo);
        for (int i = 0; i < quantidade; i++) {
            if (lerPlaca(arquivo, placa) != 1) { sucesso = 0; break; }
            marcarTempo(&inicio);
            int indice = buscar(tabela, tamanho, placa);
            marcarTempo(&fim);
            ns_busca += diferencaNs(inicio, fim);
            if (indice == -1) {
                fprintf(stderr, "Erro: busca nao encontrou %s.\n", placa);
                sucesso = 0;
                break;
            }
            buscas_atual++;
        }
        if (!sucesso) break;

        rewind(arquivo);
        for (int i = 0; i < quantidade; i++) {
            if (lerPlaca(arquivo, placa) != 1) { sucesso = 0; break; }
            marcarTempo(&inicio);
            int resultado = excluir(tabela, tamanho, placa);
            marcarTempo(&fim);
            ns_exclusao += diferencaNs(inicio, fim);
            if (resultado != 1) {
                fprintf(stderr, "Erro: nao foi possivel excluir %s.\n", placa);
                sucesso = 0;
                break;
            }
            exclusoes_atual++;
        }
        if (!sucesso) break;

        if (repeticao == 0) {
            rewind(arquivo);
            for (int i = 0; i < quantidade; i++) {
                if (lerPlaca(arquivo, placa) != 1 ||
                    buscar(tabela, tamanho, placa) != -1) {
                    fprintf(stderr, "Erro: uma placa permaneceu apos exclusao.\n");
                    sucesso = 0;
                    break;
                }
            }
            colisoes_relatorio = colisoes;
            inseridas = insercoes_atual;
            encontradas = buscas_atual;
            excluidas = exclusoes_atual;
        }
    }

    if (sucesso) {
        double tempo_insercao_ms = ns_insercao / (REPETICOES * 1000000.0);
        double tempo_busca_ms = ns_busca / (REPETICOES * 1000000.0);
        double tempo_exclusao_ms = ns_exclusao / (REPETICOES * 1000000.0);

        printf("\n--- %d placas | tabela: %zu posicoes ---\n", quantidade, tamanho);
        printf("Inseridas: %d | Buscadas: %d | Excluidas: %d\n",
               inseridas, encontradas, excluidas);
        printf("Colisoes na insercao: %llu\n", colisoes_relatorio);
        printf("Tempo medio de insercao: %.6f ms\n", tempo_insercao_ms);
        printf("Tempo medio de busca:    %.6f ms\n", tempo_busca_ms);
        printf("Tempo medio de exclusao: %.6f ms\n", tempo_exclusao_ms);

        fprintf(csv, "%d,%zu,%llu,%.6f,%.6f,%.6f,%d,%d,%d\n",
                quantidade, tamanho, colisoes_relatorio,
                tempo_insercao_ms, tempo_busca_ms, tempo_exclusao_ms,
                inseridas, encontradas, excluidas);
    }

    free(tabela);
    return sucesso;
}

int main(int argc, char *argv[]) {
    const char *caminho_entrada = (argc >= 2) ? argv[1] : "dados_placas.txt";
    const char *caminho_csv = (argc >= 3) ? argv[2] : "resultados_hash.csv";
    const int quantidades[] = {64, 128, 256, 512, 1000};

    if (argc > 3) {
        fprintf(stderr, "Uso: %s [arquivo_de_placas] [arquivo_csv]\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE *arquivo = fopen(caminho_entrada, "rb");
    if (arquivo == NULL) {
        fprintf(stderr, "Nao foi possivel abrir %s.\n", caminho_entrada);
        return EXIT_FAILURE;
    }

    FILE *csv = fopen(caminho_csv, "w");
    if (csv == NULL) {
        fprintf(stderr, "Nao foi possivel criar %s.\n", caminho_csv);
        fclose(arquivo);
        return EXIT_FAILURE;
    }
    fprintf(csv, "placas,tamanho_tabela,colisoes,tempo_insercao_ms,"
                 "tempo_busca_ms,tempo_exclusao_ms,inseridas,encontradas,excluidas\n");

    printf("LAB Hash - dispersao dupla - %d repeticoes por entrada\n", REPETICOES);
    printf("Arquivo de entrada: %s\n", caminho_entrada);

    int sucesso = 1;
    size_t quantidade_testes = sizeof(quantidades) / sizeof(quantidades[0]);
    for (size_t i = 0; i < quantidade_testes; i++) {
        if (!executarTeste(arquivo, csv, quantidades[i])) {
            sucesso = 0;
            break;
        }
    }

    if (fclose(csv) != 0) sucesso = 0;
    fclose(arquivo);

    if (!sucesso) {
        fprintf(stderr, "Um ou mais testes falharam.\n");
        return EXIT_FAILURE;
    }

    printf("\nResultados gravados em: %s\n", caminho_csv);
    return EXIT_SUCCESS;
}