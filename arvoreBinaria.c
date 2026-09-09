#include <stdio.h>
#include <stdlib.h>

struct no {
    int chave;
    int altura;
    struct no *esq;
    struct no *dir;
};

typedef struct no No;
No *criaNo(int chave) {
    No *novo = (No *) malloc(sizeof(No));
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


No *insere(No *raiz, int chave) {
    if (raiz == NULL) {
        return criaNo(chave);
    }
    if (chave < raiz->chave) {
        raiz->esq = insere(raiz->esq, chave);
    }
    else if (chave > raiz->chave) {
        raiz->dir = insere(raiz->dir, chave);
    }
    return raiz;
}

int calculaAltura(No *raiz) {

    if (raiz == NULL) {
        return -1;
    }

    int alturaE = calculaAltura(raiz->esq);
    int alturaD = calculaAltura(raiz->dir);

    if (alturaE > alturaD) {
        raiz->altura = alturaE + 1;
    }
    else {
        raiz->altura = alturaD + 1;
    }

    return raiz->altura;
}

void preOrdem(No *raiz) {

    if (raiz != NULL) {
        printf("%d(%d) ", raiz->chave, raiz->altura);

        preOrdem(raiz->esq);
        preOrdem(raiz->dir);
    }
}

void simetrica(No *raiz) {

    if (raiz != NULL) {
        simetrica(raiz->esq);

        printf("%d(%d) ", raiz->chave, raiz->altura);

        simetrica(raiz->dir);
    }
}

void liberaArvore(No *raiz) {

    if (raiz != NULL) {
        liberaArvore(raiz->esq);
        liberaArvore(raiz->dir);

        free(raiz);
    }
}


int main(void) {

    FILE *arquivo;
    No *raiz = NULL;
    int chave;

    arquivo = fopen("chaves.txt", "r");

    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo chaves.txt\n");
        return 1;
    }

    while (fscanf(arquivo, "%d", &chave) == 1) {
        raiz = insere(raiz, chave);
    }

    fclose(arquivo);


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