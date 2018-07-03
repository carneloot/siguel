#ifndef __SORTS_H__
#define __SORTS_H__

void heap_sort(
  void **arr, int size, int (*compare)(const void *, const void *));

void merge_sort(
  void **arr, int size, int (*compare)(const void *, const void *));

#endif /* __SORTS_H__ */