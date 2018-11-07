#ifndef __GRAFOD_H__
#define __GRAFOD_H__

#include <stdbool.h>
#include "lista.h"

/**
 * GrafoD é uma representacao de um Dígrafo em C
 */
typedef void *GrafoD;

/**
 * InfoG é a informacao que tem dentro de um GrafoD
 */
typedef void *InfoG;

struct GrafoD_t {

  /**
   * Cria um grafo sem nenhuma aresta e o numero de vertices definido.
   */
  GrafoD (*create)(int numero_vertices);

  /**
   * Funcao para destruir e desalocar o grafo.
   * Todos os vertices e arestas existentes irao ser perdidos.
   */
  void (*destroy)(GrafoD this);

  /* ===== FUNCOES ARESTA ===== */

  /**
   * Insere no grafo this uma aresta que liga os vertices node1 e node2.
   * A direcao eh node1 -> node2
   * Os vertices node1 e node2 devem existir;
   */
  void (*insert_aresta)(GrafoD this, char *node1, char *node2);

  /**
   * Define a informacao ligada a aresta node1->node2.
   * A aresta deve existir
   */
  void (*define_info_aresta)(GrafoD this, char *node1, char *node2, InfoG info);

  /**
   * Retorna a informacao ligada a aresta node1->node2.
   * A aresta deve existir.
   */
  InfoG (*get_info_aresta)(GrafoD this, char *node1, char *node2);

  /**
   * Remove a aresta node1->node2 do grafo this.
   * A aresta nao deve mais partencer ao grafo.
   */
  void (*remove_aresta)(GrafoD this, char *node1, char *node2);

  /**
   * Retorna true se node1 aponta para node2. Falso caso contrario.
   */
  bool (*adjacente)(GrafoD this, char *node1, char *node2);

  /* ===== FUNCOES VERTICE ===== */

  /**
   * Insere um vertice sem informacao no grafo this.
   */
  void (*insert_vertice)(GrafoD this, char *node);

  /**
   * Define a informacao ligada ao vertice node.
   * O vertice deve existir.
   */
  void (*define_info_vertice)(GrafoD this, char *node, InfoG info);

  /**
   * Retorna a informacao ligada ao vertice node;
   * O vertice deve existir.
   */
  InfoG (*get_info_vertice)(GrafoD this, char *node);

  /**
   * Remove o vertice node do grafo this.
   * O vertice nao pode ter nenhuma ligacao a ele ou para ele.
   * O vertice nao ira mais pertencer a this.
   */
  void (*remove_vertice)(GrafoD this, char *node);

  /**
   * Retorna uma lista ligada com todos os vertices adjacentes a node.
   * O vertice deve existir.
   */
  Lista (*adjacentes)(GrafoD this, char *node);

};

extern const struct GrafoD_t GrafoD_t;

#endif