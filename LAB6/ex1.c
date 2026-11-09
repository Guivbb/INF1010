
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

int main() {
    int i;
    t_no *p;
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
    p = f1;

    while (p != NULL) {
        for (i = 0; i < p->ndesc; i++)
            printf("%d ", p->chave[i]);
        p = p->prox;
    }

    printf("\n");

    free(arv);
    free(esq);
    free(dir);
    free(f1);
    free(f2);
    free(f3);
    free(f4);
    free(f5);
    free(f6);

    return 0;
}
