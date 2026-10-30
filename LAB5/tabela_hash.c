#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TABLE_SIZE 1511   // Número primo maior que 1000 para bom fator de carga
#define PLATE_LEN 8       // 7 caracteres da placa + '\0'

// Enum para o status de cada posição do vetor (Endereçamento Interno)
typedef enum {
    EMPTY = 0,
    OCCUPIED = 1,
    DELETED = 2
} Status;

typedef struct {
    char placa[PLATE_LEN];
    Status status;
} HashEntry;

// Variável global para contabilizar colisões totais
unsigned long long total_colisoes = 0;

// Função Hash 1 (Método de Horner para strings)
unsigned long hash1(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    return hash % TABLE_SIZE;
}

// Função Hash 2 para Dispersão Dupla (Gera um passo co-primo com TABLE_SIZE)
unsigned long hash2(const char *str) {
    unsigned long hash = 0;
    while (*str) {
        hash = (hash * 37) + (*str);
        str++;
    }
    unsigned long step = 1 + (hash % (TABLE_SIZE - 2));
    return step;
}

// Inicializa a tabela hash
void inicializarTabela(HashEntry tabela[]) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        tabela[i].status = EMPTY;
        tabela[i].placa[0] = '\0';
    }
}

// Algoritmo de Inclusão com Dispersão Dupla
int inserir(HashEntry tabela[], const char *placa) {
    unsigned long h1 = hash1(placa);
    unsigned long h2 = hash2(placa);

    for (int i = 0; i < TABLE_SIZE; i++) {
        unsigned long idx = (h1 + i * h2) % TABLE_SIZE;

        if (tabela[idx].status == EMPTY || tabela[idx].status == DELETED) {
            strcpy(tabela[idx].placa, placa);
            tabela[idx].status = OCCUPIED;
            return 1; // Sucesso
        } else if (tabela[idx].status == OCCUPIED) {
            if (strcmp(tabela[idx].placa, placa) == 0) {
                return 0; // Placa duplicada
            }
            total_colisoes++; // Ocorreu colisão
        }
    }
    return -1; // Tabela cheia
}

// Algoritmo de Busca
int buscar(HashEntry tabela[], const char *placa) {
    unsigned long h1 = hash1(placa);
    unsigned long h2 = hash2(placa);

    for (int i = 0; i < TABLE_SIZE; i++) {
        unsigned long idx = (h1 + i * h2) % TABLE_SIZE;

        if (tabela[idx].status == EMPTY) {
            return -1; // Não encontrado
        }
        if (tabela[idx].status == OCCUPIED && strcmp(tabela[idx].placa, placa) == 0) {
            return (int)idx; // Encontrado
        }
        // Se estiver DELETED, continua a sondagem
    }
    return -1; // Não encontrado após percorrer a tabela
}

// Algoritmo de Exclusão (Lazy Deletion)
int excluir(HashEntry tabela[], const char *placa) {
    int idx = buscar(tabela, placa);
    if (idx != -1) {
        tabela[idx].status = DELETED;
        return 1; // Removido com sucesso
    }
    return 0; // Não encontrado
}

// Função para limpar quebras de linha indesejadas (CR/LF)
void limparString(char *str) {
    size_t len = strlen(str);
    while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r')) {
        str[len - 1] = '\0';
        len--;
    }
}

int main() {
    HashEntry *tabela = (HashEntry *)malloc(TABLE_SIZE * sizeof(HashEntry));
    if (!tabela) {
        printf("Erro de alocacao de memoria!\n");
        return 1;
    }

    inicializarTabela(tabela);

    FILE *file = fopen("placas.txt", "r");
    if (!file) {
        printf("Erro ao abrir o arquivo 'placas.txt'. Certifique-se de que ele esta no diretorio.\n");
        free(tabela);
        return 1;
    }

    char buffer[32];
    int limiteEntrada = 1000; // Altere para testar 64, 128, 256, 512, 1000
    int count = 0;

    printf("Iniciando insercao de placas...\n");
    
    clock_t inicio_tempo = clock();

    while (fgets(buffer, sizeof(buffer), file) != NULL && count < limiteEntrada) {
        limparString(buffer);
        if (strlen(buffer) >= 7) {
            inserir(tabela, buffer);
            count++;
        }
    }
    fclose(file);

    clock_t fim_tempo = clock();
    double tempo_inclusao = (double)(fim_tempo - inicio_tempo) * 1000.0 / CLOCKS_PER_SEC;

    // Relatório de Inclusão
    printf("\n=== RELATORIO DE INCLUSAO (%d placas) ===\n", count);
    printf("Total de Colisoes Geradas: %llu\n", total_colisoes);
    printf("Tempo de Inclusao: %.4f ms\n", tempo_inclusao);

    // Fase de Busca (conforme exigido: após o término da inclusão)
    printf("\nIniciando fase de busca para todas as placas inseridas...\n");
    file = fopen("placas.txt", "r");
    int acertos = 0;
    count = 0;
    
    clock_t inicio_busca = clock();
    while (fgets(buffer, sizeof(buffer), file) != NULL && count < limiteEntrada) {
        limparString(buffer);
        if (strlen(buffer) >= 7) {
            if (buscar(tabela, buffer) != -1) {
                acertos++;
            }
            count++;
        }
    }
    fclose(file);
    clock_t fim_busca = clock();
    double tempo_busca = (double)(fim_busca - inicio_busca) * 1000.0 / CLOCKS_PER_SEC;

    printf("Buscas realizadas com sucesso: %d/%d\n", acertos, count);
    printf("Tempo total de Busca: %.4f ms\n", tempo_busca);

    free(tabela);
    return 0;
}