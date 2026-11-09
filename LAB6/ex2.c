#include <stdio.h>
#include <stdlib.h>

#define MAX 4
#define MIN 2

typedef struct no t_no;

struct no {
    int ndesc;
    int chave[MAX];
    t_no *ramo[MAX + 1];
    t_no *prox;
};

t_no *cria_no(int n) {
    t_no *p = malloc(sizeof(t_no));
    int i;

    if (p == NULL) {
        printf("Erro ao alocar memoria\n");
        exit(1);
    }
    p->ndesc = n;
    p->prox = NULL;
    for (i = 0; i < MAX + 1; i++)
        p->ramo[i] = NULL;
    return p;
}

t_no *busca_folha(t_no *arv, int x) {
    t_no *p = arv;
    int i;

    while (p->ramo[0] != NULL) {
        i = 0;
        while (i < p->ndesc && x >= p->chave[i])
            i++;
        p = p->ramo[i];
    }
    return p;
}

int busca(t_no *arv, int x) {
    t_no *folha = busca_folha(arv, x);
    int i;

    for (i = 0; i < folha->ndesc; i++)
        if (folha->chave[i] == x)
            return 1;
    return 0;
}

void intervalo(t_no *arv, int lim_inf, int lim_sup) {
    t_no *p = busca_folha(arv, lim_inf);
    int i;

    while (p != NULL) {
        for (i = 0; i < p->ndesc; i++) {
            if (p->chave[i] >= lim_sup)
                return;
            if (p->chave[i] > lim_inf)
                printf("%d ", p->chave[i]);
        }
        p = p->prox;
    }
}

int main() {
    t_no *f1 = cria_no(2);
    f1->chave[0] = 10;
    f1->chave[1] = 40;

    t_no *f2 = cria_no(3);
    f2->chave[0] = 50;
    f2->chave[1] = 60;
    f2->chave[2] = 70;

    t_no *f3 = cria_no(3);
    f3->chave[0] = 75;
    f3->chave[1] = 80;
    f3->chave[2] = 90;

    t_no *f4 = cria_no(3);
    f4->chave[0] = 100;
    f4->chave[1] = 110;
    f4->chave[2] = 115;

    t_no *f5 = cria_no(4);
    f5->chave[0] = 120;
    f5->chave[1] = 135;
    f5->chave[2] = 140;
    f5->chave[3] = 170;

    t_no *f6 = cria_no(4);
    f6->chave[0] = 200;
    f6->chave[1] = 230;
    f6->chave[2] = 240;
    f6->chave[3] = 250;

    f1->prox = f2;
    f2->prox = f3;
    f3->prox = f4;
    f4->prox = f5;
    f5->prox = f6;

    t_no *esq = cria_no(2);
    esq->chave[0] = 50;
    esq->chave[1] = 75;
    esq->ramo[0] = f1;
    esq->ramo[1] = f2;
    esq->ramo[2] = f3;

    t_no *dir = cria_no(2);
    dir->chave[0] = 120;
    dir->chave[1] = 200;
    dir->ramo[0] = f4;
    dir->ramo[1] = f5;
    dir->ramo[2] = f6;

    t_no *arv = cria_no(1);
    arv->chave[0] = 100;
    arv->ramo[0] = esq;
    arv->ramo[1] = dir;

    if (busca(arv, 230))
        printf("a) A chave 230 esta na arvore.\n");
    else
        printf("a) A chave 230 nao esta na arvore.\n");

    printf("b) Chaves em [50,170]: ");
    intervalo(arv, 49, 171);
    printf("\n");

    return 0;
}