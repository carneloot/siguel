#ifndef __HASH_H__
#define __HASH_H__

#include <stdio.h>

typedef void *HashTable_t;

/**
 * Cria a HashTable_t. Use tamanho, de preferencia, numeros primos.
 */
HashTable_t ht_create(int tamanho);

/**
 * Destroi a HashTable_t this;
 */
void ht_destroy(HashTable_t this, void (*destruir_item)(void *item), int destruir_chave);

/**
 * Insere valor na hashtable de acordo com a chave
 */
void ht_insert(HashTable_t this, char *chave, void *valor);

/**
 * Retorna verdadeiro caso exista um registro com a chave passada.
 */
int ht_exists(HashTable_t this, char *chave);

/**
 * Retorna o registro cuja chave é a passada.
*/
void *ht_get(HashTable_t this, char *chave);

/**
 * Retorna o numero de casas preenchidas da HashTable_t
 */
int ht_length(HashTable_t this);

/**
 * Retorna o total de casas existentes na HashTable_t
 */
int ht_max_size(HashTable_t this);

/**
 * A chave nao pode existir mais
 */
void ht_remove(HashTable_t this, char *chave);

/**
 * Printa a tabela em fp de acordo com a funcao to_string
 */
void ht_print(
  HashTable_t this, char *(*to_string)(void *valor), FILE *fp);

/**
 * Mapeia uma funcao pela lista inteira
 */
void ht_map(HashTable_t this, void *other, void (*map_function)(void *valor, void *other));

#endif /* __HASH_H__ */