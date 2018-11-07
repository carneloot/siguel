#include "grafod.h"

#include <assert.h>
#include <stdlib.h>

#include "hash.h"
#include "kdtree.h"

struct GrafoD {
  int num_vertices;

  KDTree vertices;
  HashTable vertices_x_arestas;
};

static bool __validar_grafo(struct GrafoD *this) {
  if (this->vertices == NULL)
    return false;
  
  if (this->vertices_x_arestas == NULL)
    return false;

  if (this->num_vertices == 0)
    return false;
  
  return true;
}

static GrafoD __create_grafod(int numero_vertices) {
  struct GrafoD *this = calloc(1, sizeof(*this));

  this->num_vertices = numero_vertices;
  this->vertices     = NULL;

  int total_hashtable = (int) (numero_vertices / 0.7) + 1;

  this->vertices_x_arestas = HashTable_t.create(total_hashtable);
  
  return this;
}

static void __destroy_grafod(GrafoD _this) {
  struct GrafoD *this = (struct GrafoD *) _this;

  if (this->vertices)
    KDTree_t.destroy(this->vertices, free);
  
  HashTable_t.destroy(this->vertices_x_arestas, 0, false);

  free(this);
}

static void __definir_funcoes_grafod(GrafoD _this,
    int (*check_equal)(const void *a, const void *b),
    int (*compare_x)(const void *a, const void *b),
    int (*comapre_y)(const void *a, const void *b)
  ) {
  struct GrafoD *this = (struct GrafoD *) _this;

  this->vertices = KDTree_t.create(2, check_equal, compare_x, comapre_y);
  
}

/* ===== FUNCOES ARESTA ===== */

static void __insert_aresta_grafod(GrafoD this, char *node1, char *node2) {}

static void __define_info_aresta_grafod(GrafoD this, char *node1, char *node2, InfoG info) {}

static InfoG __get_info_aresta_grafod(GrafoD this, char *node1, char *node2) {}

static void __remove_aresta_grafod(GrafoD this, char *node1, char *node2) {}

static bool __adjacente_grafod(GrafoD this, char *node1, char *node2) {}

/* ===== FUNCOES VERTICE ===== */

static void __insert_vertice_grafod(GrafoD this, char *node) {}

static void __define_info_vertice_grafod(GrafoD this, char *node, InfoG info) {}

static InfoG __get_info_vertice_grafod(GrafoD this, char *node) {}

static void __remove_vertice_grafod(GrafoD this, char *node) {}

static Lista __adjacentes_grafod(GrafoD this, char *node) {}

const struct GrafoD_t GrafoD_t = { //
  .create              = &__create_grafod,
  .insert_aresta       = &__insert_aresta_grafod,
  .define_info_aresta  = &__define_info_aresta_grafod,
  .get_info_aresta     = &__get_info_aresta_grafod,
  .remove_aresta       = &__remove_aresta_grafod,
  .adjacente           = &__adjacente_grafod,
  .insert_vertice      = &__insert_vertice_grafod,
  .define_info_vertice = &__define_info_vertice_grafod,
  .get_info_vertice    = &__get_info_vertice_grafod,
  .remove_vertice      = &__remove_vertice_grafod,
  .adjacentes          = &__adjacentes_grafod,
  .destroy             = &__destroy_grafod,
  .definir_funcoes     = &__definir_funcoes_grafod,
};