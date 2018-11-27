#ifndef __PQUEUE_H__
#define __PQUEUE_H__

typedef void *PQueue;

/**
 * Cria uma fila
 */
PQueue pq_create(int capacidade);

/**
 * Retorna a quantidade de itens na fila
 */
int pq_getsize(PQueue this);

/**
 * Retorna o item de menor prioridade
 */
void *pq_getmin(PQueue this);

/**
 * Remove o e retorna menor elemento de menor prioridade
 */
void *pq_extractmin(PQueue this);

/**
 * Muda a prioridade de value para prioridade
 */
void pq_decrease(PQueue this, double priority, void *value);

/**
 * Insere value na fila com prioridade priority
 */
void pq_insert(PQueue this, double priority, void *value);

/**
 * Remove value da fila
 */
void pq_remove(PQueue this, void *value);

/**
 * Destroi a fila
 */
void pq_destroy(PQueue this, void (*destroy_item)(void *item));

#endif /* __PQUEUE_H__ */