#ifndef __SORTS_H__
#define __SORTS_H__

void quick_sort(void **arr, int ini, int fim, int (*compare)(void *, void *));

void heap_sort(void **arr, int size, int (*compare)(const void *, const void *));

#endif /* __SORTS_H__ */