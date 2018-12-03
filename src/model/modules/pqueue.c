#include "pqueue.h"

#include <stdlib.h>
#include <limits.h>

#define parent(i) (((i) - 1) / 2)
#define left(i)   (2 * (i) + 1)
#define right(i)  (2 * (i) + 2)

struct PItem {
  double priority;
  void *value;
};

struct PQueue {
  struct PItem *array;
  int size;
  int capacity;
};


static void swap(struct PItem *a, struct PItem *b) {
  struct PItem aux;
  aux = *a;
  *a  = *b;
  *b  = aux;
}

static void heapify(struct PItem *arr, int size, int i) {
  if (i >= size / 2)
    return;

  int menor = i;
  int esq = left(i);
  int dir = right(i);

  if (esq >= size)
    esq = menor;

  if (dir >= size)
    dir = menor;

  if (arr[esq].priority < arr[menor].priority)
    menor = esq;

  if (arr[dir].priority < arr[menor].priority)
    menor = dir;

  if (i == menor)
    return;

  swap(&arr[i], &arr[menor]);
  heapify(arr, size, menor);
}

static int findIndex(struct PQueue *this, void *value) {
  int i;
  for (i = 0; i < this->size; i++) {
    if (this->array[i].value == value)
      break;
  }
  return i;
}

PQueue pq_create(int capacity) {
  struct PQueue *this = calloc(1, sizeof(*this));

  this->array    = calloc(capacity, sizeof(*this->array));
  this->size     = 0;
  this->capacity = capacity;
  return this;
}

int pq_getsize(PQueue _this) {
  struct PQueue * this = (struct PQueue *) _this;
  return this->size;
}

void *pq_getmin(PQueue _this) {
  struct PQueue * this = (struct PQueue *) _this;
  return this->array[0].value;
}

void *pq_extractmin(PQueue _this) {
  struct PQueue * this = (struct PQueue *) _this;

  if (this->size <= 0)
    return NULL;
  
  if (this->size == 1) {
    this->size--;
    return this->array[0].value;
  }

  struct PItem raiz = this->array[0];
  this->array[0] = this->array[this->size - 1];
  this->size--;
  heapify(this->array, this->size, 0);

  return raiz.value;
}

void pq_decrease(PQueue _this, double priority, void *value) {
  struct PQueue * this = (struct PQueue *) _this;
  
  int indice = findIndex(this, value);
  
  this->array[indice].priority = priority;

  while (indice != 0 &&
    this->array[parent(indice)].priority > this->array[indice].priority) {
    
    swap(&this->array[indice], &this->array[parent(indice)]);
    indice = parent(indice);
  }
}

void pq_insert(PQueue _this, double priority, void *value) {
  struct PQueue *this = (struct PQueue *) _this;

  if (this->size >= this->capacity) return;
  this->size++;

  // Coloca no final do vetor
  int indice = this->size - 1;
  this->array[indice].priority = priority;
  this->array[indice].value = value;

  // Arrumar o heap
  while (indice != 0 &&
    this->array[parent(indice)].priority > this->array[indice].priority ) {
    
    swap(&this->array[indice], &this->array[parent(indice)]);
    indice = parent(indice);
  }
  
}

void pq_remove(PQueue _this, void *value) {
  pq_decrease(_this, INT_MIN, value);
  pq_extractmin(_this);
}

void pq_destroy(PQueue _this, void (*destroy_item)(void *item)) {
  struct PQueue * this = (struct PQueue *) _this;
  
  if (destroy_item) {
    for (int i = 0; i < this->size; i++) {
      destroy_item(this->array[i].value);
    }
  }

  free(this->array);
  free(this);
}