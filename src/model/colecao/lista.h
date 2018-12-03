#ifndef __LISTA_H__
#define __LISTA_H__

/**
 * Uma lista é coleção ordenada de itens em que seus elementos podem ser
 * acessados atraves de sua posicao. Uma lista sem nenhum elemento (comprimento
 * zero) é denominada lista vazia.
 */
typedef void *Lista_t;

/**
 * Indica a posicao de um item dentro da lista. Ese tipo possui um valor
 * invalido para qualquer lista. Tal valor invalido é denotado por NULL.
 */
typedef void *Posic_t;

/**
 * Retorna uma lista vazia.
 */
Lista_t lt_create();

/**
 * Retorna o numero de elementos da lista.
 */
int lt_length(Lista_t this);

/**
 * Insere o item no final da lista. Retorna um indicador para o elemento
 * acrescentado.
 */
Posic_t lt_insert(Lista_t this, void *item);

/**
 * A posicao deve ser um elemento existente em L.
 * Remove o elemento da lista indicado pela posicao. O indicador deixa de ser
 * válido da lista.
 */
void *lt_remove(Lista_t this, Posic_t posicao);

/**
 * Retorna o valor do item que a posicao indica.
 */
void *lt_get(Lista_t this, Posic_t posicao);

/**
 * Insere o item info na posicao imediatamente anterior ao item indicado pela
 * posicao. Retorna um indicador para o elemento adicionado.
 */
Posic_t lt_insert_before(Lista_t this, Posic_t posicao, void *item);

/**
 * Insere o item info na posicao imediatamente posterior ao item indicado pela
 * posicao. Retorna um indicador para o elemento adicionado.
 */
Posic_t lt_insert_after(Lista_t this, Posic_t posicao, void *item);

/**
 * Retorna o indicador do primeiro elemento existente em L.
 * Se size(lista) == 0, retorna NULL.
 */
Posic_t lt_get_first(Lista_t this);

/**
 * Retorna o indicador imediatamente seguinte ao indicado pela posicao.
 * Se a posicao for a ultima, retorna NULL.
 */
Posic_t lt_get_next(Lista_t this, Posic_t posicao);

/**
 * Retorna o indicador do ultimo elemento existente em L.
 * Se size(Lista) == 0, retorna NULL.
 */
Posic_t lt_get_last(Lista_t this);

/**
 * Retorna o indicador imediatamente anterior ao indicado pela posicao.
 * Se a posicao for a ultima, retorna NULL.
 */
Posic_t lt_get_previous(Lista_t this, Posic_t posicao);

/**
 * Retorna o indicador da primeira posicao a partir de start que encontrar o
 * elemento de acordo com a funcao compare. Se nao encontrar um void *que caiba
 * na funcao compare retorna um indicador invalido.
 */
Posic_t lt_search(
  Lista_t lista,
  Posic_t start,
  void *other,
  int (*compare)(void *item, void *other));

/**
 * Retorna uma copia da lista passada.
 */
Lista_t lt_copy(Lista_t this);

/**
 * Retorna uma lista filtrada de acordo com a funcao passada
 */
Lista_t lt_filter(Lista_t this, void *other, int (*compare)(void *item, void *other));

/**
 * Mapeia uma funcao para todos os itens da lista.
 */
void lt_map(Lista_t this, void *other, void (*map_function)(void *item, void *other));

/**
 * Retorna uma array de void *com todos os itens da lista.
 * O tamanho do array é o mesmo da lista. Deve ser dado free no retorno.
 */
void **lt_to_array(Lista_t this);

/**
 * Retorna 1 se a lista contem o item, 0 caso contrario.
 */
int lt_contem(Lista_t this, void *item);

/**
 * Destroi a lista.
 */
void lt_destroy(Lista_t this, void (*destroy_item)(void *item));

#endif /* __LISTA_H__ */