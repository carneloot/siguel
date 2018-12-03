#ifndef __GRAFOD_H__
#define __GRAFOD_H__

#include <stdbool.h>
#include <stdio.h>
#include "lista.h"

/**
 * GrafoD_t é uma representacao de um Dígrafo em C
 */
typedef void *GrafoD_t;

/**
 * Cria um grafo sem nenhuma aresta e o numero de vertices definido.
 */
GrafoD_t gd_create();

/**
 * Funcao para destruir e desalocar o grafo.
 * Todos os vertices e arestas existentes irao ser perdidos.
 */
void gd_destroy(GrafoD_t this);

/**
 * Funcao para retornar o label de todos os vertices existentes
 */
char **gd_get_all_vertices(GrafoD_t this);

/**
 * Funcao para retornar o label de todos os vertices existentes
 */
int gd_total_vertices(GrafoD_t this);

/**
 * Funcao para gerar um arquivo GraphViz do grafo
 */
void gd_generate_dot(GrafoD_t this, FILE *fp);

/* ===== FUNCOES ARESTA ===== */

/**
 * Insere no grafo this uma aresta que liga os vertices origem e destino.
 * A direcao eh origem -> destino
 * Os vertices origem e destino devem existir;
 */
void gd_insert_aresta(GrafoD_t this, char *origem, char *destino);

/**
 * Define a informacao ligada a aresta origem->destino.
 * A aresta deve existir
 */
void gd_define_info_aresta(GrafoD_t this, char *origem, char *destino, void *info);

/**
 * Retorna a informacao ligada a aresta origem->destino.
 * A aresta deve existir.
 */
void *gd_get_info_aresta(GrafoD_t this, char *origem, char *destino);

/**
 * Remove a aresta origem->destino do grafo this.
 * A aresta nao deve mais partencer ao grafo.
 */
void gd_remove_aresta(GrafoD_t this, char *origem, char *destino);

/**
 * Retorna true se origem aponta para destino. Falso caso contrario.
 */
bool gd_adjacente(GrafoD_t this, char *origem, char *destino);

/* ===== FUNCOES VERTICE ===== */

/**
 * Insere um vertice sem informacao no grafo this.
 */
void gd_insert_vertice(GrafoD_t this, char *node);

/**
 * Define a informacao ligada ao vertice node.
 * O vertice deve existir.
 */
void gd_define_info_vertice(GrafoD_t this, char *node, void *info);

/**
 * Retorna a informacao ligada ao vertice node;
 * O vertice deve existir.
 */
void *gd_get_info_vertice(GrafoD_t this, char *node);

/**
 * Remove o vertice node do grafo this.
 * O vertice nao pode ter nenhuma ligacao a ele ou para ele.
 * O vertice nao ira mais pertencer a this.
 */
void gd_remove_vertice(GrafoD_t this, char *node);

/**
 * Retorna uma lista ligada com os labels de todos os vertices adjacentes a node.
 * O vertice deve existir.
 */
Lista_t gd_adjacentes(GrafoD_t this, char *node);

#endif