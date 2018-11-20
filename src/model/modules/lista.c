#include "lista.h"

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

static Lista __create_lista() {
  struct Lista *this = (struct Lista *) malloc(sizeof(struct Lista));

  this->start = NULL;
  this->end   = NULL;
  this->size  = 0;

  return (Lista) this;
}

static int __length_lista(Lista lista) {
  struct Lista *this = (struct Lista *) lista;
  return this->size;
}

static Posic __insert_lista(Lista lista, Item item) {
  struct Lista *this = (struct Lista *) lista;
  struct Posic *posic;

  posic = (struct Posic *) malloc(sizeof(struct Posic));

  posic->next  = NULL;
  posic->prev  = NULL;
  posic->value = item;

  // Se a lista estiver vazia
  if (__length_lista(lista) == 0) {
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

static void __remove_lista(Lista lista, Posic posicao) {
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

static Item __get_lista(Lista lista, Posic posicao) {
  struct Posic *posic = (struct Posic *) posicao;

  if (!posicao)
    return NULL;

  return posic->value;
}

static Posic __insert_before_lista(Lista lista, Posic posicao, Item item) {
  struct Lista *this     = (struct Lista *) lista;
  struct Posic *posic    = (struct Posic *) posicao;
  struct Posic *newPosic = (struct Posic *) malloc(sizeof(struct Posic));

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

  return (Posic) newPosic;
}

static Posic __insert_after_lista(Lista lista, Posic posicao, Item item) {
  struct Lista *this     = (struct Lista *) lista;
  struct Posic *posic    = (struct Posic *) posicao;
  struct Posic *newPosic = (struct Posic *) malloc(sizeof(struct Posic));

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

  return (Posic) newPosic;
}

static Posic __get_first_lista(Lista lista) {
  struct Lista *this = (struct Lista *) lista;
  return (Posic) this->start;
}

static Posic __get_next_lista(Lista lista, Posic posicao) {
  struct Posic *posic = (struct Posic *) posicao;
  if (!posicao)
    return NULL;
  return (Posic) posic->next;
}

static Posic __get_last_lista(Lista lista) {
  struct Lista *this = (struct Lista *) lista;
  return (Posic) this->end;
}

static Posic __get_previous_lista(Lista lista, Posic posicao) {
  struct Posic *posic = (struct Posic *) posicao;
  if (!posicao)
    return NULL;
  return (Posic) posic->prev;
}

static Posic __search_lista(
  Lista _lista,
  Posic _start,
  const void *other,
  int (*compare)(const Item item, const void *other)) {
  struct Posic *iterator;

  iterator = (struct Posic *) _start;

  while (iterator) {
    if (!compare(iterator->value, other))
      return (Posic) iterator;

    iterator = iterator->next;
  }

  return NULL;
}

static Lista __copy_lista(Lista _this) {
  struct Lista * this = (struct Lista *) _this;
  Lista copia = __create_lista();

  struct Posic *it = this->start;
  while (it) {
    __insert_lista(copia, it->value);
    it = it->next;
  }

  return copia;
}

static Lista __filter_lista(Lista _this, void *other, int (*compare)(const Item item, const void *other)) {
  struct Lista * this = (struct Lista *) _this;
  Lista filtrado = __create_lista();

  struct Posic *it = __get_first_lista(this);
  it = __search_lista(this, it, other, compare);
  while (it) {
    Item item = __get_lista(this, it);

    __insert_lista(filtrado, item);

    it = __search_lista(this, __get_next_lista(this, it), other, compare);
  }
  
  return filtrado;
}

static void __map_lista(Lista _this, void *other, void (*map_function)(const Item item, const void *other)) {
  struct Lista * this = (struct Lista *) _this;

  struct Posic *it = __get_first_lista(this);
  while (it) {
    Item item = __get_lista(this, it);

    map_function(item, other);

    it = __get_next_lista(this, it);
  }
  
}

static Item *__to_array_lista(Lista _this) {
  struct Lista *this = (struct Lista *) _this;

  Item *array = (Item *) calloc(this->size, sizeof(Item));

  struct Posic *iterator = this->start;

  int i = 0;
  while (iterator) {
    array[i] = iterator->value;

    iterator = iterator->next;
    i++;
  }

  return array;
}

static int __contem_lista(Lista _this, Item item) {
  struct Lista *this = (struct Lista *) _this;

  Posic it = __get_first_lista(this);

  while (it) {
    Item atual = __get_lista(this, it);

    if (atual == item) {
      return 1;
    }

    it = __get_next_lista(this, it);
  }

  return 0;
}

static void __destruir_lista(Lista lista, void (*destruir_item)(Item item)) {
  if (destruir_item) {
    Posic iterator;

    iterator = __get_first_lista(lista);
    while (iterator) {
      destruir_item(__get_lista(lista, iterator));
      iterator = __get_next_lista(lista, iterator);
    }
  }

  // Destruir os nodes que ainda existam.
  while (__length_lista(lista) != 0)
    __remove_lista(lista, __get_last_lista(lista));

  free(lista);
}

const struct Lista_t Lista_t = {  //
  .create        = &__create_lista,
  .length        = &__length_lista,
  .insert        = &__insert_lista,
  .remove        = &__remove_lista,
  .get           = &__get_lista,
  .insert_before = &__insert_before_lista,
  .insert_after  = &__insert_after_lista,
  .get_first     = &__get_first_lista,
  .get_next      = &__get_next_lista,
  .get_last      = &__get_last_lista,
  .get_previous  = &__get_previous_lista,
  .search        = &__search_lista,
  .copy          = &__copy_lista,
  .filter        = &__filter_lista,
  .map           = &__map_lista,
  .to_array      = &__to_array_lista,
  .contem        = &__contem_lista,
  .destruir      = &__destruir_lista};