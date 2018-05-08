#include "lista.h"

#include <stdlib.h>

struct Node {
  Item value;
  struct Node *next;
  struct Node *prev;
};

struct Lista {
  struct Node *start;
  struct Node *end;
  int tamanho;
};

Lista cria_lista() {
  struct Lista *this;

  this = (struct Lista *) malloc(sizeof(struct Lista));

  this->start   = NULL;
  this->end     = NULL;
  this->tamanho = 0;

  return (void *) this;
}

int lista_vazia(Lista l) {
  return (!((struct Lista *) l)->tamanho);
}

void inserir_lista(Lista l, Item item) {
  struct Lista *this;
  struct Node *node;

  this = (struct Lista *) l;
  node = (struct Node *) malloc(sizeof(struct Node));

  node->value = item;
  node->next  = NULL;
  node->prev  = NULL;

  this->tamanho++;

  if (!this->start) {
    this->start = node;
    this->end   = node;
    return;
  }

  node->prev      = this->end;
  this->end->next = node;
  this->end       = node;
}

Item remover_inicio_lista(Lista l) {
  struct Lista *this;
  struct Node *node;
  Item item;

  this = (struct Lista *) l;
  node = this->start;
  item = node->value;

  this->start = this->start->next;
  if (this->start)
    this->start->prev = NULL;

  free(node);

  this->tamanho--;

  return item;
}

void destruir_lista(Lista l) {
  struct Node *atual;
  struct Lista *this;

  this = (struct Lista *) l;

  if (lista_vazia(l)) {
    free(this);
    return;
  }

  atual = this->end->prev;

  while (atual) {
    free(atual->next->value);
    free(atual->next);

    atual = atual->prev;
  }

  free(this->start->value);
  free(this->start);

  free(this);
}

void para_cada_lista(Lista l, void (*f)(Item)) {
  struct Lista *this;
  struct Node *atual;

  this  = (struct Lista *) l;
  atual = this->start;

  while (atual) {
    (*f)(atual->value);

    atual = atual->next;
  }
}

Node get_start_lista(Lista l) {
  return (Node)((struct Lista *) l)->start;
}

/** Manupulacao do Nó */

int tem_proximo_node(Node node) {
  return (!!node);
}

Item get_proximo_node(Node *nodeP) {
  struct Node *node;
  Item item;

  node = (struct Node *) *nodeP;

  item = node->value;

  *nodeP = (Node) node->next;

  return item;
}