
#ifndef __KDTREE_H__
#define __KDTREE_H__

#include <stdarg.h>
#include <stdio.h>
#include "lista.h"

typedef void *KDTree_t;

struct Pair {
  double distance;
  void *point1;
  void *point2;
};

typedef struct Pair Pair;

/**
 * Funcao para criar a KDTree_t;
 * @param k: numero para particionar
 * resto dos parametros são as funções de comparacao do modelo
 * int compare(void *a, void *b)
 */
KDTree_t kdt_create(
  int dimensao, int (*check_equal)(void *a, void *b), ...);

/**
 * Insere na arvore, usando as funcoes passadas no inicio
 */
KDTree_t kdt_insert(KDTree_t this, void *value);

/**
 * Remove recursivamente o nó de valor 'value' da arvore 'this'
 */
void kdt_remove(KDTree_t this, void *value);

/**
 * Procura na arvore 'this' o nó com valor 'value'. Usa a funcao check para
 * checar o valor
 */
KDTree_t kdt_search(
  KDTree_t this, void *value, int (*check)(void *a, void *b));

/**
 * Retorna o valor da raiz da KDTree_t passada.
 */
void *kdt_get(KDTree_t this);

/**
 * Retorna 1 caso a KDTree_t indicada for uma folha.
 * 0 caso nao seja.
 */
int kdt_is_leaf(KDTree_t this);

/**
 * Retorna 1 caso a KDTree_t indicada esteja vazia.
 * 0 caso nao esteja.
 */
int kdt_is_empty(KDTree_t this);

/**
 * Passa por toda a arvore executando a funcao nos itens
 */
void kdt_passe_simetrico(
  KDTree_t this,
  void (*executar)(void *item, unsigned profundidade, va_list list),
  ...);

/**
 * Pesquisa na arvore 'this' de acordo com um intervalo
 * A funcao 'dentro' checa a posicao do item value de acordo com a dimensao
 * 'dim'. Se 'dim' for -1, checa se o valor esta completamente dentro do
 * intervalo
 */
Lista_t kdt_range_search(
  KDTree_t this, int (*dentro)(void *value, int dim, void *ponto_a, void *ponto_b), void *ponto_a, void *ponto_b);

/**
 * Acha o ponto mais proximo do valor value
 * A funcao 'distance' acha a distancia entre 'a' e 'b' na dimensao 'dim'. Se
 * dim for -1, acha a distancia de acordo com todas as dimensoes
 */
Pair kdt_nearest_neighbor(
  KDTree_t this,
  void *value,
  double (*distance)(void *a, void *b, int dim));

/**
 * Retorna o par mais proximo dentro da KDTree_t de acordo com a funco distance
 */
Pair kdt_closest_pair(
  KDTree_t this, double (*distance)(void *a, void *b, int dim));

/**
 * Gera e exporta um arquivo .dot no arquivo fp
 */
void kdt_generate_dot(
  KDTree_t this, FILE *fp, char *(*to_string)(void *item));

/**
 * Destroi a arvore
 */
void kdt_destroy(KDTree_t this, void (*destroy_element)(void *item));

#endif  // __KDTREE_H__
