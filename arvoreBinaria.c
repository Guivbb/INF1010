#include <stdio.h>
#include <stdlib.h>

struct nodo {
    int chave;
    int altura;
    struct nodo *esq;
    struct nodo *dir;
};

typedef struct nodo Nodo;


/* Cria um novo nó */
Nodo *criaNodo(int chave) {
    Nodo *novo = (Nodo *) malloc(sizeof(Nodo));

    if (novo == NULL) {
        printf("Erro ao alocar memoria!\n");
        exit(1);
    }

    novo->chave = chave;
    novo->altura = 0;
    novo->esq = NULL;
    novo->dir = NULL;

    return novo;
}


/* Insere uma chave na árvore binária de busca */
Nodo *inserir(Nodo *raiz, int chave) {

    if (raiz == NULL) {
        return criaNodo(chave);
    }

    if (chave < raiz->chave) {
        raiz->esq = inserir(raiz->esq, chave);
    }
    else if (chave > raiz->chave) {
        raiz->dir = inserir(raiz->dir, chave);
    }

    return raiz;
}


/* Calcula e armazena a altura de cada nó */
int calculaAltura(Nodo *raiz) {

    if (raiz == NULL) {
        return -1;
    }

    int alturaEsq = calculaAltura(raiz->esq);
    int alturaDir = calculaAltura(raiz->dir);

    if (alturaEsq > alturaDir) {
        raiz->altura = alturaEsq + 1;
    }
    else {
        raiz->altura = alturaDir + 1;
    }

    return raiz->altura;
}


/* Percurso em pré-ordem:
   raiz -> esquerda -> direita
*/
void preOrdem(Nodo *raiz) {

    if (raiz != NULL) {
        printf("%d(%d) ", raiz->chave, raiz->altura);

        preOrdem(raiz->esq);
        preOrdem(raiz->dir);
    }
}


/* Percurso em ordem simétrica:
   esquerda -> raiz -> direita
*/
void simetrica(Nodo *raiz) {

    if (raiz != NULL) {
        simetrica(raiz->esq);

        printf("%d(%d) ", raiz->chave, raiz->altura);

        simetrica(raiz->dir);
    }
}


/* Libera a memória da árvore */
void liberaArvore(Nodo *raiz) {

    if (raiz != NULL) {
        liberaArvore(raiz->esq);
        liberaArvore(raiz->dir);

        free(raiz);
    }
}


int main(void) {

    FILE *arquivo;
    Nodo *raiz = NULL;
    int chave;

    arquivo = fopen("entrada.txt", "r");

    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo entrada.txt\n");
        return 1;
    }

    /* Lê todas as chaves do arquivo */
    while (fscanf(arquivo, "%d", &chave) == 1) {
        raiz = inserir(raiz, chave);
    }

    fclose(arquivo);

    /* Calcula a altura de cada nó */
    calculaAltura(raiz);

    printf("Pre-ordem: ");
    preOrdem(raiz);

    printf("\n");

    printf("Simetrica: ");
    simetrica(raiz);

    printf("\n");

    liberaArvore(raiz);

    return 0;
}