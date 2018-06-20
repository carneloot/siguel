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
typedef void *Item;

/**
 * Retorna uma lista vazia.
 */
Lista create_lista();

/**
 * Retorna o numero de elementos da lista.
 */
int length_lista(Lista);

/**
 * Insere o item no final da lista. Retorna um indicador para o elemento
 * acrescentado.
 */
Posic insert_lista(Lista, Item);

/**
 * A posicao deve ser um elemento existente em L.
 * Remove o elemento da lista indicado pela posicao. O indicador deixa de ser
 * válido da lista.
 */
void remove_lista(Lista, Posic);

/**
 * Retorna o valor do item que a posicao indica.
 */
Item get_lista(Lista, Posic);

/**
 * Insere o item info na posicao imediatamente anterior ao item indicado pela
 * posicao. Retorna um indicador para o elemento adicionado.
 */
Posic insert_before_lista(Lista, Posic, Item);

/**
 * Insere o item info na posicao imediatamente posterior ao item indicado pela
 * posicao. Retorna um indicador para o elemento adicionado.
 */
Posic insert_after_lista(Lista, Posic, Item);

/**
 * Retorna o indicador do primeiro elemento existente em L.
 * Se length_lista(Lista) == 0, retorna NULL.
 */
Posic get_first_lista(Lista);

/**
 * Retorna o indicador imediatamente seguinte ao indicado pela posicao.
 * Se a posicao for a ultima, retorna NULL.
 */
Posic get_next_lista(Lista, Posic);

/**
 * Retorna o indicador do ultimo elemento existente em L.
 * Se length_lista(Lista) == 0, retorna NULL.
 */
Posic get_last_lista(Lista);

/**
 * Retorna o indicador imediatamente anterior ao indicado pela posicao.
 * Se a posicao for a ultima, retorna NULL.
 */
Posic get_previous_lista(Lista, Posic);

/**
 * Destroi a lista.
 */
void destruir_lista(Lista, void (*)(Item));

#endif /* __LISTA_H__ */