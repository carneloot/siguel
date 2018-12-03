#include "grafod.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <model/colecao/hash.h>

#include <utils/logger.h>

/** ===== ARESTA ===== */

struct Aresta {
  void *info;

  char *label_origem;
  char *label_destino;
};

static struct Aresta *create_aresta(void *info, char *label_origem, char *label_destino) {
  struct Aresta *this = calloc(1, sizeof(*this));

  this->info = info;

  this->label_origem = label_origem;

  this->label_destino = label_destino;

  return this;
}

static void destroy_aresta(void *_this) {
  struct Aresta *this = _this;
  free(this);
}

/** ===== VERTICE ===== */

struct Vertice {
  void *info;

  char *label;
  HashTable_t arestas;
};

static struct Vertice *create_vertice(void *info, char *label) {
  struct Vertice *this = calloc(1, sizeof(*this));

  this->info = info;

  this->label = calloc(strlen(label) + 1, sizeof(char));
  strcpy(this->label, label);

  this->arestas = ht_create(7);

  return this;
}

static void destroy_vertice(void *_this) {
  struct Vertice *this = _this;
  ht_destroy(this->arestas, destroy_aresta, 0);

  free(this->label);

  free(this);
}

/** ===== GRAFO DIRECIONADO ===== */

struct GrafoD_t {
  int num_vertices;

  Lista_t vertices;
  HashTable_t label_x_vertice;
};

GrafoD_t gd_create() {
  struct GrafoD_t *this = calloc(1, sizeof(*this));

  this->num_vertices = 0;

  this->vertices = lt_create();
  this->label_x_vertice = ht_create(199);
  
  return this;
}

void gd_destroy(GrafoD_t _this) {
  struct GrafoD_t *this = (struct GrafoD_t *) _this;

  lt_destroy(this->vertices, NULL);
  ht_destroy(this->label_x_vertice, destroy_vertice, false);

  free(this);
}

char **gd_get_all_vertices(GrafoD_t _this) {
  struct GrafoD_t *this = (struct GrafoD_t *) _this;

  char **retorno = calloc(this->num_vertices, sizeof(*retorno));

  int indice = 0;

  Posic_t it = lt_get_first(this->vertices);

  while (it) {
    struct Vertice *vertice = lt_get(this->vertices, it);

    retorno[indice++] = vertice->label;

    it = lt_get_next(this->vertices, it);
  }

  return retorno;
}

int gd_total_vertices(GrafoD_t _this) {
  struct GrafoD_t *this = (struct GrafoD_t *) _this;

  return this->num_vertices;
}

void __imprimir_aresta_dot(void *_aresta, void *_fp) {
  struct Aresta *aresta = (struct Aresta *) _aresta;
  FILE *fp = (FILE *) _fp;

  fprintf(fp, "\"%p\" -> \"%p\";\n", aresta->label_origem, aresta->label_destino);

}

void gd_generate_dot(GrafoD_t _this, FILE *fp) {
  struct GrafoD_t *this = _this;
  fprintf(fp, "digraph T {\n");
  fprintf(fp, "graph [rankdir=LR];\n");
  fprintf(fp, "node [fontname=\"Arial\"];\n");
  fprintf(fp, "edge [shape=\"normal\"];\n");

  // Passar por todos os vertices imprimindo as arestas

  Posic_t it = lt_get_first(this->vertices);

  while (it) {
    struct Vertice *vertice = lt_get(this->vertices, it);
    
    fprintf(fp, "\"%p\" [label=\"%s\"];\n", vertice->label, vertice->label);

    ht_map(vertice->arestas, fp, __imprimir_aresta_dot);

    it = lt_get_next(this->vertices, it);
  }

  fprintf(fp, "}\n");
}

/* ===== FUNCOES ARESTA ===== */

void gd_insert_aresta(GrafoD_t _this, char *origem, char *destino) {
  struct GrafoD_t *this = _this;

  if (!ht_exists(this->label_x_vertice, origem)) {
    LOG_ERRO("ERRO INSERIR ARESTA: Vertice de nome \"%s\" nao existe.", origem);
    return;
  }

  if (!ht_exists(this->label_x_vertice, destino)) {
    LOG_ERRO("ERRO INSERIR ARESTA: Vertice de nome \"%s\" nao existe.", destino);
    return;
  }

  struct Vertice *vertice_origem  = ht_get(this->label_x_vertice, origem);
  struct Vertice *vertice_destino = ht_get(this->label_x_vertice, destino);

  struct Aresta *aresta = create_aresta(NULL, vertice_origem->label, vertice_destino->label);

  ht_insert(vertice_origem->arestas, vertice_destino->label, aresta);

}

void gd_define_info_aresta(GrafoD_t _this, char *origem, char *destino, void *info) {
  struct GrafoD_t *this = _this;

  if (!ht_exists(this->label_x_vertice, origem)) {
    LOG_ERRO("ERRO DEFINIR INFO ARESTA: Vertice de nome \"%s\" nao existe.", origem);
    return;
  }

  if (!ht_exists(this->label_x_vertice, destino)) {
    LOG_ERRO("ERRO DEFINIR INFO ARESTA: Vertice de nome \"%s\" nao existe.", destino);
    return;
  }

  struct Vertice *vertice_origem = ht_get(this->label_x_vertice, origem);

  if (!ht_exists(vertice_origem->arestas, destino)) {
    LOG_ERRO(
      "ERRO DEFINIR INFO ARESTA: "
      "Aresta de \"%s\" a \"%s\" nao existe. Por favor, crie a "
      "aresta antes de atribuir uma informacao a ela.", origem, destino);
    return;
  }

  struct Aresta *aresta = ht_get(vertice_origem->arestas, destino);

  aresta->info = info;
}

void *gd_get_info_aresta(GrafoD_t _this, char *origem, char *destino) {
  struct GrafoD_t *this = _this;

  if (!ht_exists(this->label_x_vertice, origem)) {
    LOG_ERRO("ERRO GET INFO ARESTA: Vertice de nome \"%s\" nao existe.", origem);
    return NULL;
  }

  if (!ht_exists(this->label_x_vertice, destino)) {
    LOG_ERRO("ERRO GET INFO ARESTA: Vertice de nome \"%s\" nao existe.", destino);
    return NULL;
  }

  struct Vertice *vertice_origem = ht_get(this->label_x_vertice, origem);

  if (!ht_exists(vertice_origem->arestas, destino)) {
    LOG_ERRO(
      "ERRO GET INFO ARESTA: "
      "Aresta de \"%s\" a \"%s\" nao existe. Por favor, crie a "
      "aresta antes de acessa-la.", origem, destino);
    return NULL;
  }

  struct Aresta *aresta = ht_get(vertice_origem->arestas, destino);

  return aresta->info;
}

void gd_remove_aresta(GrafoD_t _this, char *origem, char *destino) {
  struct GrafoD_t *this = _this;

  if (!ht_exists(this->label_x_vertice, origem)) {
    LOG_ERRO("ERRO REMOVER ARESTA: Vertice de nome \"%s\" nao existe.", origem);
    return;
  }

  if (!ht_exists(this->label_x_vertice, destino)) {
    LOG_ERRO("ERRO REMOVER ARESTA: Vertice de nome \"%s\" nao existe.", destino);
    return;
  }

  struct Vertice *vertice_origem = ht_get(this->label_x_vertice, origem);

  if (!ht_exists(vertice_origem->arestas, destino)) {
    LOG_ERRO(
      "ERRO REMOVER ARESTA: "
      "Aresta de \"%s\" a \"%s\" nao existe. Por favor, crie a "
      "aresta antes de remove-la.", origem, destino);
    return;
  }

  struct Aresta *aresta = ht_get(vertice_origem->arestas, destino);

  ht_remove(vertice_origem->arestas, destino);

  destroy_aresta(aresta);
}

bool gd_adjacente(GrafoD_t _this, char *origem, char *destino) {
  struct GrafoD_t *this = _this;

  if (!ht_exists(this->label_x_vertice, origem)) {
    LOG_ERRO("ERRO CHECAR ADJACENTE: Vertice de nome \"%s\" nao existe.", origem);
    return false;
  }

  if (!ht_exists(this->label_x_vertice, destino)) {
    LOG_ERRO("ERRO CHECAR ADJACENTE: Vertice de nome \"%s\" nao existe.", destino);
    return false;
  }

  struct Vertice *vertice_origem = ht_get(this->label_x_vertice, origem);

  return ht_exists(vertice_origem->arestas, destino);
}

/* ===== FUNCOES VERTICE ===== */

void gd_insert_vertice(GrafoD_t _this, char *node) {
  struct GrafoD_t *this = _this;

  if (ht_exists(this->label_x_vertice, node)) {
    LOG_ERRO("ERRO INSERIR VERTICE: Vertice com nome \"%s\" ja existe.", node);
    return;
  }

  struct Vertice *vertice = create_vertice(NULL, node);

  lt_insert(this->vertices, vertice);
  ht_insert(this->label_x_vertice, vertice->label, vertice);

  this->num_vertices++;
}

void gd_define_info_vertice(GrafoD_t _this, char *node, void *info) {
  struct GrafoD_t *this = _this;

  if (!ht_exists(this->label_x_vertice, node)) {
    LOG_ERRO("ERRO DEFINIR INFO VERTICE: Vertice de nome \"%s\" nao existe.", node);
    return;
  }

  struct Vertice *vertice = ht_get(this->label_x_vertice, node);

  vertice->info = info;
}

void *gd_get_info_vertice(GrafoD_t _this, char *node) {
  struct GrafoD_t *this = _this;

  if (!ht_exists(this->label_x_vertice, node)) {
    LOG_ERRO("ERRO GET INFO VERTICE: Vertice de nome \"%s\" nao encontrado.", node);
    return NULL;
  }

  struct Vertice *vertice = ht_get(this->label_x_vertice, node);

  return vertice->info;
}

void gd_remove_vertice(GrafoD_t _this, char *node) {
  struct GrafoD_t *this = _this;

  if (!ht_exists(this->label_x_vertice, node)) {
    LOG_ERRO("ERRO REMOVER VERTICE: Vertice com nome \"%s\" nao encontrado.", node);
    return;
  }

  struct Vertice *vertice = ht_get(this->label_x_vertice, node);

  // Checar se tem alguma ligacao saindo dele
  if (ht_length(vertice->arestas) != 0) {
    LOG_ERRO("ERRO REMOVER VERTICE: Vertice \"%s\" contem arestas, e nao pode ser removido.", node);
    return;
  }

  // Checar se tem alguma ligacao chegando nele
  // Todo: 

  ht_remove(this->label_x_vertice, node);

  destroy_vertice(vertice);
}

void __adicionar_lista(void *_aresta, void *_lista) {
  const struct Aresta *aresta = (const struct Aresta *) _aresta;
  Lista_t lista = _lista;

  lt_insert(lista, aresta->label_destino);
}

Lista_t gd_adjacentes(GrafoD_t _this, char *node) {
  struct GrafoD_t *this = _this;

  Lista_t lista = lt_create();

  struct Vertice *vertice = ht_get(this->label_x_vertice, node);

  ht_map(vertice->arestas, lista, __adicionar_lista);

  return lista;
}
