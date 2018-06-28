#include "modules/lista.h"

#include <stdlib.h>

struct Posic {
  Item value;
  struct Posic *next;
  struct Posic *prev;
};

struct Lista {
  struct Posic *start;
  struct Posic *end;
  int size;
};

Lista create_lista() {
  struct Lista *this = (struct Lista *) malloc(sizeof(struct Lista));

  this->start = NULL;
  this->end   = NULL;
  this->size  = 0;

  return (Lista) this;
}

int length_lista(Lista lista) {
  struct Lista *this = (struct Lista *) lista;
  return this->size;
}

Posic insert_lista(Lista lista, Item info) {
  struct Lista *this = (struct Lista *) lista;
  struct Posic *posic;

  posic = (struct Posic *) malloc(sizeof(struct Posic));

  posic->next  = NULL;
  posic->prev  = NULL;
  posic->value = info;

  // Se a lista estiver vazia
  if (length_lista(lista) == 0) {
    this->start = posic;
    this->end   = posic;
    this->size++;

    return posic;
  }

  posic->prev     = this->end;
  this->end->next = posic;
  this->end       = posic;

  this->size++;

  return posic;
}

void remove_lista(Lista lista, Posic posicao) {
  struct Lista *this  = (struct Lista *) lista;
  struct Posic *posic = (struct Posic *) posicao;

  if (!posicao)
    return;

  if (this->start == posic)
    this->start = posic->next;

  if (this->end == posic)
    this->end = posic->prev;

  if (posic->prev)
    posic->prev->next = posic->next;

  if (posic->next)
    posic->next->prev = posic->prev;

  free(posic);

  this->size--;
}

Item get_lista(Lista lista, Posic posicao) {
  struct Posic *posic = (struct Posic *) posicao;

  if (!posicao)
    return NULL;

  return posic->value;
}

Posic insert_before_lista(Lista lista, Posic posicao, Item info) {
  struct Lista *this     = (struct Lista *) lista;
  struct Posic *posic    = (struct Posic *) posicao;
  struct Posic *newPosic = (struct Posic *) malloc(sizeof(struct Posic));

  if (!posicao)
    return NULL;

  newPosic->prev  = NULL;
  newPosic->next  = posic;
  newPosic->value = info;

  if (this->start == posic) {
    this->start->prev = newPosic;
    this->start       = newPosic;

    this->size++;
    return newPosic;
  }

  posic->prev->next = newPosic;
  newPosic->prev    = posic->prev;
  posic->prev       = newPosic;

  this->size++;

  return (Posic) newPosic;
}

Posic insert_after_lista(Lista lista, Posic posicao, Item info) {
  struct Lista *this     = (struct Lista *) lista;
  struct Posic *posic    = (struct Posic *) posicao;
  struct Posic *newPosic = (struct Posic *) malloc(sizeof(struct Posic));

  if (!posicao)
    return NULL;

  newPosic->prev  = posic;
  newPosic->next  = NULL;
  newPosic->value = info;

  if (this->end == posic) {
    this->end->next = newPosic;
    this->end       = newPosic;

    this->size++;
    return newPosic;
  }

  posic->next->prev = newPosic;
  newPosic->next    = posic->next;
  posic->next       = newPosic;

  this->size++;

  return (Posic) newPosic;
}

Posic get_first_lista(Lista lista) {
  struct Lista *this = (struct Lista *) lista;
  return (Posic) this->start;
}

Posic get_next_lista(Lista lista, Posic posicao) {
  struct Posic *posic = (struct Posic *) posicao;
  if (!posicao)
    return NULL;
  return (Posic) posic->next;
}

Posic get_last_lista(Lista lista) {
  struct Lista *this = (struct Lista *) lista;
  return (Posic) this->end;
}

Posic get_previous_lista(Lista lista, Posic posicao) {
  struct Posic *posic = (struct Posic *) posicao;
  if (!posicao)
    return NULL;
  return (Posic) posic->prev;
}

Posic search_lista(
  Lista _lista,
  Posic _start,
  const void *other,
  int (*compare)(const Item item, const void *other)) {
  struct Lista *lista = (struct Lista *) _lista;
  struct Posic *iterator;

  iterator = (struct Posic *) _start;
    
  while (iterator) {
    if (!compare(iterator->value, other))
      return (Posic) iterator;

    iterator = iterator->next;
  }

  return NULL;
}

void destruir_lista(Lista lista, void (*destruir_item)(Item item)) {
  if (destruir_item) {
    Posic iterator;

    iterator = get_first_lista(lista);
    while (iterator) {
      destruir_item(get_lista(lista, iterator));
      iterator = get_next_lista(lista, iterator);
    }
  }

  // Destruir os nodes que ainda existam.
  while (length_lista(lista) != 0)
    remove_lista(lista, get_last_lista(lista));

  free(lista);
}