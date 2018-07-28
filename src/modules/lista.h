#ifndef __LISTA_H__
#define __LISTA_H__

/**
 * Uma lista é coleção ordenada de itens em que seus elementos podem ser
 * acessados atraves de sua posicao. Uma lista sem nenhum elemento (comprimento
 * zero) é denominada lista vazia.
 */
typedef void *Lista;

/**
 * Indica a posicao de um item dentro da lista. Ese tipo possui um valor
 * invalido para qualquer lista. Tal valor invalido é denotado por NULL.
 */
typedef void *Posic;

/**
 * Item da lista.
 */
#ifndef ITEM_TYPE
typedef void *Item;
#define ITEM_TYPE
#endif

struct Lista_t {
  /**
   * Retorna uma lista vazia.
   */
  Lista (*create)();

  /**
   * Retorna o numero de elementos da lista.
   */
  int (*length)(Lista);

  /**
   * Insere o item no final da lista. Retorna um indicador para o elemento
   * acrescentado.
   */
  Posic (*insert)(Lista, Item);

  /**
   * A posicao deve ser um elemento existente em L.
   * Remove o elemento da lista indicado pela posicao. O indicador deixa de ser
   * válido da lista.
   */
  void (*remove)(Lista, Posic);

  /**
   * Retorna o valor do item que a posicao indica.
   */
  Item (*get)(Lista, Posic);

  /**
   * Insere o item info na posicao imediatamente anterior ao item indicado pela
   * posicao. Retorna um indicador para o elemento adicionado.
   */
  Posic (*insert_before)(Lista, Posic, Item);

  /**
   * Insere o item info na posicao imediatamente posterior ao item indicado pela
   * posicao. Retorna um indicador para o elemento adicionado.
   */
  Posic (*insert_after)(Lista, Posic, Item);

  /**
   * Retorna o indicador do primeiro elemento existente em L.
   * Se size(lista) == 0, retorna NULL.
   */
  Posic (*get_first)(Lista);

  /**
   * Retorna o indicador imediatamente seguinte ao indicado pela posicao.
   * Se a posicao for a ultima, retorna NULL.
   */
  Posic (*get_next)(Lista, Posic);

  /**
   * Retorna o indicador do ultimo elemento existente em L.
   * Se size(Lista) == 0, retorna NULL.
   */
  Posic (*get_last)(Lista);

  /**
   * Retorna o indicador imediatamente anterior ao indicado pela posicao.
   * Se a posicao for a ultima, retorna NULL.
   */
  Posic (*get_previous)(Lista, Posic);

  /**
   * Retorna o indicador da primeira posicao a partir de start que encontrar o
   * elemento de acordo com a funcao compare. Se nao encontrar um Item que caiba
   * na funcao compare retorna um indicador invalido.
   */
  Posic (*search)(
    Lista lista,
    Posic start,
    const void *other,
    int (*compare)(const Item item, const void *other));

  /**
   * Retorna uma array de Item com todos os itens da lista.
   * O tamanho do array é o mesmo da lista. Deve ser dado free no retorno.
   */
  Item *(*to_array)(Lista lista);

  /**
   * Destroi a lista.
   */
  void (*destruir)(Lista, void (*)(Item));
};

extern const struct Lista_t Lista_t;

#endif /* __LISTA_H__ */