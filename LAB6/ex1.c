
#include <stdio.h>
#include <stdlib.h>
#define MAX 4
#define MIN 2

typedef struct no t_no;
struct no {
    char nome;
    int ndesc;
    int chave[MAX + 1];
    t_no *ramo[MAX + 1];
    t_no *prox;
};

t_no *cria_no(char nome, int n) {
    t_no *p = malloc(sizeof(t_no));
    int i;
    if (p == NULL) {
        printf("Erro ao alocar memoria\n");
        exit(1);
    }
    p->nome = nome;
    p->ndesc = n;
    p->prox = NULL;
    for (i = 0; i < MAX + 1; i++) {
        p->chave[i] = -1;
        p->ramo[i] = NULL;
    }
    return p;
}

void imprime_no(t_no *p) {
    int i;
    printf("%-5c | ", p->nome);
    for (i = 0; i < MAX + 1; i++) {
        if (p->ramo[i] == NULL)
            printf("%-5s ", "NULL");
        else
            printf("%-5c ", p->ramo[i]->nome);
    }
    printf("| ");
    for (i = 0; i < MAX + 1; i++) {
        printf("%d", p->chave[i]);
        if (i < MAX)
            printf(", ");
    }
    printf("\n");
}

int main() {
    t_no *a = cria_no('a', 2);
    a->chave[0] = 10;
    a->chave[1] = 40;

    t_no *b = cria_no('b', 3);
    b->chave[0] = 50;
    b->chave[1] = 60;
    b->chave[2] = 70;

    t_no *c = cria_no('c', 3);
    c->chave[0] = 75;
    c->chave[1] = 80;
    c->chave[2] = 90;

    t_no *d = cria_no('d', 3);
    d->chave[0] = 100;
    d->chave[1] = 110;
    d->chave[2] = 115;

    t_no *e = cria_no('e', 4);
    e->chave[0] = 120;
    e->chave[1] = 135;
    e->chave[2] = 140;
    e->chave[3] = 170;

    t_no *f = cria_no('f', 4);
    f->chave[0] = 200;
    f->chave[1] = 230;
    f->chave[2] = 240;
    f->chave[3] = 250;

    a->prox = b;
    b->prox = c;
    c->prox = d;
    d->prox = e;
    e->prox = f;

    t_no *x = cria_no('x', 2);
    x->chave[0] = 50;
    x->chave[1] = 75;

    x->ramo[0] = a;
    x->ramo[1] = b;
    x->ramo[2] = c;

    t_no *y = cria_no('y', 2);
    y->chave[0] = 120;
    y->chave[1] = 200;
    y->ramo[0] = d;
    y->ramo[1] = e;
    y->ramo[2] = f;

    t_no *arv = cria_no('r', 1);
    arv->chave[0] = 100;
    arv->ramo[0] = x;
    arv->ramo[1] = y;

    printf("Endereco | Ponteiros                      | Chaves\n");
    printf("------------------------------------------------------------\n");
    imprime_no(arv);
    imprime_no(x);
    imprime_no(y);
    imprime_no(a);
    imprime_no(b);
    imprime_no(c);
    imprime_no(d);
    imprime_no(e);
    imprime_no(f);
    return 0;
}
