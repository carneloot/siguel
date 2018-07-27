#include "sorts.h"

#include <stdlib.h>

static void heapify(
  void **arr, int size, int i, int (*compare)(const void *, const void *));

static void merge(
  void **arr,
  int ini,
  int meio,
  int fim,
  int (*compare)(const void *, const void *));

static void merge_s(
  void **arr, int ini, int fim, int (*compare)(const void *, const void *));

static void swap(void **a, void **b);

void swap(void **a, void **b) {
  void *aux;
  aux = *a;
  *a  = *b;
  *b  = aux;
}

void heap_sort(
  void **arr, int size, int (*compare)(const void *, const void *)) {
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

void merge_sort(
  void **arr, int size, int (*compare)(const void *, const void *)) {
  merge_s(arr, 0, size - 1, compare);
}

static void heapify(
  void **arr, int size, int i, int (*compare)(const void *, const void *)) {
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

static void merge_s(
  void **arr, int ini, int fim, int (*compare)(const void *, const void *)) {
  if (ini >= fim)
    return;

  int meio = (ini + fim) / 2;

  merge_s(arr, ini, meio, compare);
  merge_s(arr, meio + 1, fim, compare);

  merge(arr, ini, meio, fim, compare);
}

void merge(
  void **arr,
  int ini,
  int meio,
  int fim,
  int (*compare)(const void *, const void *)) {
  int i, j, k;

  int n1 = meio - ini + 1;
  int n2 = fim - meio;

  // Criando e setando arrays temporarias
  void *L[n1], *R[n2];
  for (i = 0; i < n1; i++)
    L[i] = arr[ini + i];
  for (i = 0; i < n2; i++)
    R[i] = arr[meio + 1 + i];

  // Junta as arrays temporarias de volta no vetorzao
  i = j = 0;
  k     = ini;
  while (i < n1 && j < n2) {
    if (compare(L[i], R[j]) < 0)
      arr[k] = L[i++];
    else
      arr[k] = R[j++];
    k++;
  }

  // Coloca o resto que sobrou no vetorzao
  while (i < n1)
    arr[k++] = L[i++];

  while (j < n2)
    arr[k++] = R[j++];
}
