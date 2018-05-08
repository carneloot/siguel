#ifndef __LISTA_H__
#define __LISTA_H__

typedef void *Lista;
typedef void *Item;
typedef void *Node;

/** Manipulacao da lista */

Lista cria_lista();

int lista_vazia(Lista);

void inserir_lista(Lista, Item);

Item remover_inicio_lista(Lista);

void destruir_lista(Lista);

void para_cada_lista(Lista, void (*)(Item));

Node get_start_lista(Lista);

/** Manupulacao do Nó */

int tem_proximo_node(Node);

Item get_proximo_node(Node *);

#endif /* __LISTA_H__ */