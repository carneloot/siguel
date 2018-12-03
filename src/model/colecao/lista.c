#include "lista.h"

#include <stdlib.h>

struct Posic_t {
  void *value;
  struct Posic_t *next;
  struct Posic_t *prev;
};

struct Lista_t {
  struct Posic_t *start;
  struct Posic_t *end;
  int size;
};

Lista_t lt_create() {
  struct Lista_t *this = (struct Lista_t *) malloc(sizeof(*this));

  this->start = NULL;
  this->end   = NULL;
  this->size  = 0;

  return (Lista_t) this;
}

int lt_length(Lista_t lista) {
  struct Lista_t *this = (struct Lista_t *) lista;
  return this->size;
}

Posic_t lt_insert(Lista_t lista, void *item) {
  struct Lista_t *this = (struct Lista_t *) lista;
  struct Posic_t *posic = (struct Posic_t *) malloc(sizeof(*posic));

  posic->next  = NULL;
  posic->prev  = NULL;
  posic->value = item;

  // Se a lista estiver vazia
  if (lt_length(lista) == 0) {
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

void *lt_remove(Lista_t lista, Posic_t posicao) {
  struct Lista_t *this  = (struct Lista_t *) lista;
  struct Posic_t *posic = (struct Posic_t *) posicao;

  if (!posicao)
    return NULL;

  void *retorno = posic->value;

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

  return retorno;
}

void *lt_get(Lista_t lista, Posic_t posicao) {
  struct Posic_t *posic = (struct Posic_t *) posicao;

  if (!posicao)
    return NULL;

  return posic->value;
}

Posic_t lt_insert_before(Lista_t lista, Posic_t posicao, void *item) {
  struct Lista_t *this     = (struct Lista_t *) lista;
  struct Posic_t *posic    = (struct Posic_t *) posicao;
  struct Posic_t *newPosic = (struct Posic_t *) malloc(sizeof(*newPosic));

  if (!posicao)
    return NULL;

  newPosic->prev  = NULL;
  newPosic->next  = posic;
  newPosic->value = item;

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

  return newPosic;
}

Posic_t lt_insert_after(Lista_t lista, Posic_t posicao, void *item) {
  struct Lista_t *this     = (struct Lista_t *) lista;
  struct Posic_t *posic    = (struct Posic_t *) posicao;
  struct Posic_t *newPosic = (struct Posic_t *) malloc(sizeof(*newPosic));

  if (!posicao)
    return NULL;

  newPosic->prev  = posic;
  newPosic->next  = NULL;
  newPosic->value = item;

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

  return newPosic;
}

Posic_t lt_get_first(Lista_t lista) {
  struct Lista_t *this = (struct Lista_t *) lista;
  return this->start;
}

Posic_t lt_get_next(Lista_t lista, Posic_t posicao) {
  struct Posic_t *posic = (struct Posic_t *) posicao;
  if (!posicao)
    return NULL;
  return posic->next;
}

Posic_t lt_get_last(Lista_t lista) {
  struct Lista_t *this = (struct Lista_t *) lista;
  return this->end;
}

Posic_t lt_get_previous(Lista_t lista, Posic_t posicao) {
  struct Posic_t *posic = (struct Posic_t *) posicao;
  if (!posicao)
    return NULL;
  return posic->prev;
}

Posic_t lt_search(
  Lista_t _lista,
  Posic_t _start,
  void *other,
  int (*compare)(void *item, void *other)) {
  struct Posic_t *iterator;

  iterator = (struct Posic_t *) _start;

  while (iterator) {
    if (!compare(iterator->value, other))
      return iterator;

    iterator = iterator->next;
  }

  return NULL;
}

Lista_t lt_copy(Lista_t _this) {
  struct Lista_t * this = (struct Lista_t *) _this;
  Lista_t copia = lt_create();

  struct Posic_t *it = this->start;
  while (it) {
    lt_insert(copia, it->value);
    it = it->next;
  }

  return copia;
}

Lista_t lt_filter(Lista_t _this, void *other, int (*compare)(void *item, void *other)) {
  struct Lista_t * this = (struct Lista_t *) _this;
  Lista_t filtrado = lt_create();

  struct Posic_t *it = lt_get_first(this);
  it = lt_search(this, it, other, compare);
  while (it) {
    void *item = lt_get(this, it);

    lt_insert(filtrado, item);

    it = lt_search(this, lt_get_next(this, it), other, compare);
  }
  
  return filtrado;
}

void lt_map(Lista_t _this, void *other, void (*map_function)(void *item, void *other)) {
  struct Lista_t * this = (struct Lista_t *) _this;

  struct Posic_t *it = lt_get_first(this);
  while (it) {
    void *item = lt_get(this, it);

    map_function(item, other);

    it = lt_get_next(this, it);
  }
  
}

void **lt_to_array(Lista_t _this) {
  struct Lista_t *this = (struct Lista_t *) _this;

  void **array = (void **) calloc(this->size, sizeof(void *));

  struct Posic_t *iterator = this->start;

  int i = 0;
  while (iterator) {
    array[i] = iterator->value;

    iterator = iterator->next;
    i++;
  }

  return array;
}

int lt_contem(Lista_t _this, void *item) {
  struct Lista_t *this = (struct Lista_t *) _this;

  Posic_t it = lt_get_first(this);

  while (it) {
    void *atual = lt_get(this, it);

    if (atual == item) {
      return 1;
    }

    it = lt_get_next(this, it);
  }

  return 0;
}

void lt_destroy(Lista_t lista, void (*destruir_item)(void *item)) {
  if (destruir_item) {
    Posic_t iterator;

    iterator = lt_get_first(lista);
    while (iterator) {
      destruir_item(lt_get(lista, iterator));
      iterator = lt_get_next(lista, iterator);
    }
  }

  // Destruir os nodes que ainda existam.
  while (lt_length(lista) != 0)
    lt_remove(lista, lt_get_last(lista));

  free(lista);
}
