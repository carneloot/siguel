
#ifndef __KDTREE_H__
#define __KDTREE_H__

#include <stdarg.h>
#include <stdio.h>
#include "lista.h"

typedef void *KDTree;

#ifndef ITEM_TYPE
typedef void *Item;
#  define ITEM_TYPE
#endif

struct Pair {
  double distance;
  Item point1, point2;
};

typedef struct Pair Pair;

struct KDTree_t {
  /**
   * Funcao para criar a KDTree;
   * @param k: numero para particionar
   * resto dos parametros são as funções de comparacao do modelo
   * int compare(const void *a, const void *b)
   */
  KDTree (*create)(
    int dimensao, int (*check_equal)(const void *a, const void *b), ...);

  /**
   * Insere na arvore, usando as funcoes passadas no inicio
   */
  KDTree (*insert)(KDTree this, Item value);

  /**
   * Remove recursivamente o nó de valor 'value' da arvore 'this'
   */
  void (*remove)(KDTree this, Item value);

  /**
   * Procura na arvore 'this' o nó com valor 'value'. Usa a funcao check para
   * checar o valor
   */
  KDTree (*search)(
    KDTree this, Item value, int (*check)(const void *a, const void *b));

  /**
   * Retorna o valor da raiz da KDTree passada.
   */
  Item (*get)(KDTree this);

  /**
   * Retorna o node com o menor valor da dimensao 'dim'
   */
  KDTree (*find_min)(KDTree this, unsigned dim);

  /**
   * Retorna 1 caso a KDTree indicada for uma folha.
   * 0 caso nao seja.
   */
  int (*is_leaf)(KDTree this);

  /**
   * Retorna 1 caso a KDTree indicada esteja vazia.
   * 0 caso nao esteja.
   */
  int (*is_empty)(KDTree this);

  /**
   * Passa por toda a arvore executando a funcao nos itens
   */
  void (*passe_simetrico)(
    KDTree this,
    void (*executar)(const Item item, unsigned profundidade, va_list list),
    ...);

  /**
   * Pesquisa na arvore 'this' de acordo com um intervalo
   * A funcao 'dentro' checa a posicao do item value de acordo com a dimensao
   * 'dim'. Se 'dim' for -1, checa se o valor esta completamente dentro do
   * intervalo
   */
  Lista_t (*range_search)(
    KDTree this, int (*dentro)(Item value, int dim, Item ponto_a, Item ponto_b), Item ponto_a, Item ponto_b);

  /**
   * Acha o ponto mais proximo do valor value
   * A funcao 'distance' acha a distancia entre 'a' e 'b' na dimensao 'dim'. Se
   * dim for -1, acha a distancia de acordo com todas as dimensoes
   */
  Pair (*nearest_neighbor)(
    KDTree this,
    Item value,
    double (*distance)(const Item a, const Item b, int dim));

  Pair (*closest_pair)(
    KDTree this, double (*distance)(const Item a, const Item b, int dim));

  void (*generate_dot)(
    KDTree this, FILE *fp, char *(*to_string)(const Item item));

  /**
   * Destroi a arvore
   */
  void (*destroy)(KDTree this, void (*destroy_element)(Item item));
};

extern const struct KDTree_t KDTree_t;

#endif  // __KDTREE_H__
