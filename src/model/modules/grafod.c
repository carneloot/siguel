#include "grafod.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "hash.h"
#include "kdtree.h"

#include <model/modules/logger.h>

struct Aresta {
  InfoG info;

  char *label_partida;
  char *label_destino;
};

static struct Aresta *create_aresta(InfoG info, char *label_partida, char *label_destino) {
  struct Aresta *this = calloc(1, sizeof(*this));

  this->info = info;

  this->label_partida = calloc(strlen(label_partida) + 1, sizeof(char));
  strcpy(this->label_partida, label_partida);

  this->label_destino = calloc(strlen(label_destino) + 1, sizeof(char));
  strcpy(this->label_destino, label_destino);

  return this;
}

static void destroy_aresta(void *_this) {
  struct Aresta *this = _this;
  free(this->label_partida);
  free(this->label_destino);
  free(this);
}

struct Vertice {
  InfoG info;

  char *label;
  HashTable arestas;
};

static struct Vertice *create_vertice(InfoG info, char *label) {
  struct Vertice *this = calloc(1, sizeof(*this));

  this->info = info;

  this->label = calloc(strlen(label) + 1, sizeof(char));
  strcpy(this->label, label);

  this->arestas = HashTable_t.create(7);

  return this;
}

static void destroy_vertice(void *_this) {
  struct Vertice *this = _this;
  HashTable_t.destroy(this->arestas, destroy_aresta, 0);

  free(this->label);

  free(this);
}

struct GrafoD {
  int num_vertices;
  int max_vertices;

  KDTree vertices;
  HashTable label_x_vertice;
};

static bool __validar_grafo(struct GrafoD *this) {
  if (this->vertices == NULL)
    return false;
  
  if (this->label_x_vertice == NULL)
    return false;

  if (this->num_vertices == 0)
    return false;
  
  return true;
}

static GrafoD __create_grafod(int numero_vertices) {
  struct GrafoD *this = calloc(1, sizeof(*this));

  this->num_vertices = 0;
  this->max_vertices = numero_vertices;
  this->vertices     = NULL;

  int total_hashtable = (int) (numero_vertices / 0.7) + 1;

  this->label_x_vertice = HashTable_t.create(total_hashtable);
  
  return this;
}

static void __destroy_grafod(GrafoD _this) {
  struct GrafoD *this = (struct GrafoD *) _this;

  if (this->vertices)
    KDTree_t.destroy(this->vertices, destroy_vertice);
  
  HashTable_t.destroy(this->label_x_vertice, 0, false);

  free(this);
}

static void __definir_funcoes_grafod(GrafoD _this,
    int (*check_equal)(const void *a, const void *b),
    int (*compare_x)(const void *a, const void *b),
    int (*comapre_y)(const void *a, const void *b)
  ) {
  struct GrafoD *this = (struct GrafoD *) _this;

  if (this->vertices != NULL)
    return;

  this->vertices = KDTree_t.create(2, check_equal, compare_x, comapre_y);
  
}

/* ===== FUNCOES ARESTA ===== */

static void __insert_aresta_grafod(GrafoD _this, char *node1, char *node2) {}

static void __define_info_aresta_grafod(GrafoD _this, char *node1, char *node2, InfoG info) {}

static InfoG __get_info_aresta_grafod(GrafoD _this, char *node1, char *node2) {}

static void __remove_aresta_grafod(GrafoD _this, char *node1, char *node2) {}

static bool __adjacente_grafod(GrafoD _this, char *node1, char *node2) {}

/* ===== FUNCOES VERTICE ===== */

// static void __insert_vertice_grafod(GrafoD _this, char *node) {}

static void __define_info_vertice_grafod(GrafoD _this, char *node, InfoG info) {
  struct GrafoD *this = _this;

  assert(__validar_grafo(this));

  if (this->num_vertices >= this->max_vertices) {
    LOG_ERRO("Grafo cheio.");
    return;
  }

  if (HashTable_t.exists(this->label_x_vertice, node)) {
    LOG_ERRO("Vertice de nome \"%s\" ja existe.", node);
    return;
  }

  struct Vertice *vertice = create_vertice(info, node);

  KDTree_t.insert(this->vertices, vertice);

  HashInfo hash_info = {
    .chave = vertice->label,
    .valor = vertice,
  };

  HashTable_t.insert(this->label_x_vertice, hash_info);

  this->num_vertices++;

}

static InfoG __get_info_vertice_grafod(GrafoD _this, char *node) {
  struct GrafoD *this = _this;

  assert(__validar_grafo(this));

  if (!HashTable_t.exists(this->label_x_vertice, node)) {
    LOG_ERRO("Vertice de nome \"%s\" nao encontrado.", node);
    return;
  }

  struct Vertice *vertice = HashTable_t.get(this->label_x_vertice, node).valor;

  return vertice->info;
}

static void __remove_vertice_grafod(GrafoD _this, char *node) {}

static Lista __adjacentes_grafod(GrafoD _this, char *node) {}

const struct GrafoD_t GrafoD_t = { //
  .create              = &__create_grafod,
  .insert_aresta       = &__insert_aresta_grafod,
  .define_info_aresta  = &__define_info_aresta_grafod,
  .get_info_aresta     = &__get_info_aresta_grafod,
  .remove_aresta       = &__remove_aresta_grafod,
  .adjacente           = &__adjacente_grafod,
  // .insert_vertice      = &__insert_vertice_grafod,
  .define_info_vertice = &__define_info_vertice_grafod,
  .get_info_vertice    = &__get_info_vertice_grafod,
  .remove_vertice      = &__remove_vertice_grafod,
  .adjacentes          = &__adjacentes_grafod,
  .destroy             = &__destroy_grafod,
  .definir_funcoes     = &__definir_funcoes_grafod,
};