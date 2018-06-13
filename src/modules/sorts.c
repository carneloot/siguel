#include "modules/sorts.h"

static int quick_partition(
  void **arr, int ini, int fim, int (*)(void *, void *));

static void heapify(
  void **arr, int size, int i, int (*compare)(const void *, const void *));

static void swap(void **a, void **b);

void swap(void **a, void **b) {
  void *aux;
  aux = *a;
  *a  = *b;
  *b  = aux;
}

void quick_sort(
  void **arr, int ini, int fim, int (*compare)(void *x1, void *x2)) {
  int pivo;

  if (ini >= fim)
    return;

  pivo = quick_partition(arr, ini, fim, compare);

  quick_sort(arr, ini, pivo - 1, compare);
  quick_sort(arr, pivo + 1, fim, compare);
}

int quick_partition(
  void **arr, int ini, int fim, int (*compare)(void *x1, void *x2)) {
  int j, k, pivo;

  void *vPivo;

  pivo = ini + (fim - ini + 1) / 2;

  vPivo = arr[pivo];

  j = ini;
  k = fim;

  while (j < k) {
    while (compare(arr[j], vPivo))
      j++;

    while (compare(arr[k], vPivo))
      k--;

    if (arr[j] == arr[k])
      break;

    vPivo  = arr[j];
    arr[j] = arr[k];
    arr[k] = vPivo;
  }

  return j;
}

void heap_sort(void **arr, int size, int (*compare)(const void *, const void *)) {
  int i;

  // Monta o heap inicial
  for (i = size / 2 - 1; i >= 0; i--)
    heapify(arr, size, i, compare);

  // Troca o valor do ultimo e arruma o heap
  // Faz isso para todos os valores até o começo
  for (i = size - 1; i >= 0; i--) {
    swap(&arr[0], &arr[i]);
    heapify(arr, i, 0, compare);
  }
}

void heapify(void **arr, int size, int i, int (*compare)(const void *, const void *)) {
  int j, k, h;

  if (i >= size / 2)
    return;

  h = i;
  j = 2 * h + 1;
  k = 2 * h + 2;

  if (j >= size)
    j = h;

  if (k >= size)
    k = h;

  if (compare(arr[h], arr[j]) < 0)
    h = j;

  if (compare(arr[h], arr[k]) < 0)
    h = k;

  if (i == h)
    return;

  swap(&arr[i], &arr[h]);
  heapify(arr, size, h, compare);
}