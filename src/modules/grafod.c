#include "grafod.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "hash.h"

#include <modules/logger.h>

/** ===== ARESTA ===== */

struct Aresta {
  InfoG info;

  char *label_origem;
  char *label_destino;
};

static struct Aresta *create_aresta(InfoG info, char *label_origem, char *label_destino) {
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

/** ===== GRAFO DIRECIONADO ===== */

struct GrafoD {
  int num_vertices;

  Lista_t vertices;
  HashTable label_x_vertice;
};

static GrafoD __create_grafod() {
  struct GrafoD *this = calloc(1, sizeof(*this));

  this->num_vertices = 0;

  this->vertices = lt_create();
  this->label_x_vertice = HashTable_t.create(199);
  
  return this;
}

static void __destroy_grafod(GrafoD _this) {
  struct GrafoD *this = (struct GrafoD *) _this;

  lt_destroy(this->vertices, NULL);
  HashTable_t.destroy(this->label_x_vertice, destroy_vertice, false);

  free(this);
}

static char **__get_all_vertices_grafod(GrafoD _this) {
  struct GrafoD *this = (struct GrafoD *) _this;

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

static int __total_vertices_grafod(GrafoD _this) {
  struct GrafoD *this = (struct GrafoD *) _this;

  return this->num_vertices;
}

static void __imprimir_aresta_dot(void *_aresta, void *_fp) {
  struct Aresta *aresta = (struct Aresta *) _aresta;
  FILE *fp = (FILE *) _fp;

  fprintf(fp, "\"%p\" -> \"%p\";\n", aresta->label_origem, aresta->label_destino);

}

static void __generate_dot_grafod(GrafoD _this, FILE *fp) {
  struct GrafoD *this = _this;
  fprintf(fp, "digraph T {\n");
  fprintf(fp, "graph [rankdir=LR];\n");
  fprintf(fp, "node [fontname=\"Arial\"];\n");
  fprintf(fp, "edge [shape=\"normal\"];\n");

  // Passar por todos os vertices imprimindo as arestas

  Posic_t it = lt_get_first(this->vertices);

  while (it) {
    struct Vertice *vertice = lt_get(this->vertices, it);
    
    fprintf(fp, "\"%p\" [label=\"%s\"];\n", vertice->label, vertice->label);

    HashTable_t.map(vertice->arestas, fp, __imprimir_aresta_dot);

    it = lt_get_next(this->vertices, it);
  }

  fprintf(fp, "}\n");
}

/* ===== FUNCOES ARESTA ===== */

static void __insert_aresta_grafod(GrafoD _this, char *origem, char *destino) {
  struct GrafoD *this = _this;

  if (!HashTable_t.exists(this->label_x_vertice, origem)) {
    LOG_ERRO("ERRO INSERIR ARESTA: Vertice de nome \"%s\" nao existe.", origem);
    return;
  }

  if (!HashTable_t.exists(this->label_x_vertice, destino)) {
    LOG_ERRO("ERRO INSERIR ARESTA: Vertice de nome \"%s\" nao existe.", destino);
    return;
  }

  struct Vertice *vertice_origem  = HashTable_t.get(this->label_x_vertice, origem);
  struct Vertice *vertice_destino = HashTable_t.get(this->label_x_vertice, destino);

  struct Aresta *aresta = create_aresta(NULL, vertice_origem->label, vertice_destino->label);

  HashTable_t.insert(vertice_origem->arestas, vertice_destino->label, aresta);

}

static void __define_info_aresta_grafod(GrafoD _this, char *origem, char *destino, InfoG info) {
  struct GrafoD *this = _this;

  if (!HashTable_t.exists(this->label_x_vertice, origem)) {
    LOG_ERRO("ERRO DEFINIR INFO ARESTA: Vertice de nome \"%s\" nao existe.", origem);
    return;
  }

  if (!HashTable_t.exists(this->label_x_vertice, destino)) {
    LOG_ERRO("ERRO DEFINIR INFO ARESTA: Vertice de nome \"%s\" nao existe.", destino);
    return;
  }

  struct Vertice *vertice_origem = HashTable_t.get(this->label_x_vertice, origem);

  if (!HashTable_t.exists(vertice_origem->arestas, destino)) {
    LOG_ERRO(
      "ERRO DEFINIR INFO ARESTA: "
      "Aresta de \"%s\" a \"%s\" nao existe. Por favor, crie a "
      "aresta antes de atribuir uma informacao a ela.", origem, destino);
    return;
  }

  struct Aresta *aresta = HashTable_t.get(vertice_origem->arestas, destino);

  aresta->info = info;
}

static InfoG __get_info_aresta_grafod(GrafoD _this, char *origem, char *destino) {
  struct GrafoD *this = _this;

  if (!HashTable_t.exists(this->label_x_vertice, origem)) {
    LOG_ERRO("ERRO GET INFO ARESTA: Vertice de nome \"%s\" nao existe.", origem);
    return NULL;
  }

  if (!HashTable_t.exists(this->label_x_vertice, destino)) {
    LOG_ERRO("ERRO GET INFO ARESTA: Vertice de nome \"%s\" nao existe.", destino);
    return NULL;
  }

  struct Vertice *vertice_origem = HashTable_t.get(this->label_x_vertice, origem);

  if (!HashTable_t.exists(vertice_origem->arestas, destino)) {
    LOG_ERRO(
      "ERRO GET INFO ARESTA: "
      "Aresta de \"%s\" a \"%s\" nao existe. Por favor, crie a "
      "aresta antes de acessa-la.", origem, destino);
    return NULL;
  }

  struct Aresta *aresta = HashTable_t.get(vertice_origem->arestas, destino);

  return aresta->info;
}

static void __remove_aresta_grafod(GrafoD _this, char *origem, char *destino) {
  struct GrafoD *this = _this;

  if (!HashTable_t.exists(this->label_x_vertice, origem)) {
    LOG_ERRO("ERRO REMOVER ARESTA: Vertice de nome \"%s\" nao existe.", origem);
    return;
  }

  if (!HashTable_t.exists(this->label_x_vertice, destino)) {
    LOG_ERRO("ERRO REMOVER ARESTA: Vertice de nome \"%s\" nao existe.", destino);
    return;
  }

  struct Vertice *vertice_origem = HashTable_t.get(this->label_x_vertice, origem);

  if (!HashTable_t.exists(vertice_origem->arestas, destino)) {
    LOG_ERRO(
      "ERRO REMOVER ARESTA: "
      "Aresta de \"%s\" a \"%s\" nao existe. Por favor, crie a "
      "aresta antes de remove-la.", origem, destino);
    return;
  }

  struct Aresta *aresta = HashTable_t.get(vertice_origem->arestas, destino);

  HashTable_t.remove(vertice_origem->arestas, destino);

  destroy_aresta(aresta);
}

static bool __adjacente_grafod(GrafoD _this, char *origem, char *destino) {
  struct GrafoD *this = _this;

  if (!HashTable_t.exists(this->label_x_vertice, origem)) {
    LOG_ERRO("ERRO CHECAR ADJACENTE: Vertice de nome \"%s\" nao existe.", origem);
    return false;
  }

  if (!HashTable_t.exists(this->label_x_vertice, destino)) {
    LOG_ERRO("ERRO CHECAR ADJACENTE: Vertice de nome \"%s\" nao existe.", destino);
    return false;
  }

  struct Vertice *vertice_origem = HashTable_t.get(this->label_x_vertice, origem);

  return HashTable_t.exists(vertice_origem->arestas, destino);
}

/* ===== FUNCOES VERTICE ===== */

static void __insert_vertice_grafod(GrafoD _this, char *node) {
  struct GrafoD *this = _this;

  if (HashTable_t.exists(this->label_x_vertice, node)) {
    LOG_ERRO("ERRO INSERIR VERTICE: Vertice com nome \"%s\" ja existe.", node);
    return;
  }

  struct Vertice *vertice = create_vertice(NULL, node);

  lt_insert(this->vertices, vertice);
  HashTable_t.insert(this->label_x_vertice, vertice->label, vertice);

  this->num_vertices++;
}

static void __define_info_vertice_grafod(GrafoD _this, char *node, InfoG info) {
  struct GrafoD *this = _this;

  if (!HashTable_t.exists(this->label_x_vertice, node)) {
    LOG_ERRO("ERRO DEFINIR INFO VERTICE: Vertice de nome \"%s\" nao existe.", node);
    return;
  }

  struct Vertice *vertice = HashTable_t.get(this->label_x_vertice, node);

  vertice->info = info;
}

static InfoG __get_info_vertice_grafod(GrafoD _this, char *node) {
  struct GrafoD *this = _this;

  if (!HashTable_t.exists(this->label_x_vertice, node)) {
    LOG_ERRO("ERRO GET INFO VERTICE: Vertice de nome \"%s\" nao encontrado.", node);
    return NULL;
  }

  struct Vertice *vertice = HashTable_t.get(this->label_x_vertice, node);

  return vertice->info;
}

static void __remove_vertice_grafod(GrafoD _this, char *node) {
  struct GrafoD *this = _this;

  if (!HashTable_t.exists(this->label_x_vertice, node)) {
    LOG_ERRO("ERRO REMOVER VERTICE: Vertice com nome \"%s\" nao encontrado.", node);
    return;
  }

  struct Vertice *vertice = HashTable_t.get(this->label_x_vertice, node);

  // Checar se tem alguma ligacao saindo dele
  if (HashTable_t.length(vertice->arestas) != 0) {
    LOG_ERRO("ERRO REMOVER VERTICE: Vertice \"%s\" contem arestas, e nao pode ser removido.", node);
    return;
  }

  // Checar se tem alguma ligacao chegando nele
  // Todo: 

  HashTable_t.remove(this->label_x_vertice, node);

  destroy_vertice(vertice);
}

static void __adicionar_lista(void *_aresta, void *_lista) {
  const struct Aresta *aresta = (const struct Aresta *) _aresta;
  Lista_t lista = _lista;

  lt_insert(lista, aresta->label_destino);
}

static Lista_t __adjacentes_grafod(GrafoD _this, char *node) {
  struct GrafoD *this = _this;

  Lista_t lista = lt_create();

  struct Vertice *vertice = HashTable_t.get(this->label_x_vertice, node);

  HashTable_t.map(vertice->arestas, lista, __adicionar_lista);

  return lista;
}

const struct GrafoD_t GrafoD_t = { //
  .create              = &__create_grafod,
  .destroy             = &__destroy_grafod,
  .get_all_vertices    = &__get_all_vertices_grafod,
  .total_vertices      = &__total_vertices_grafod,
  .generate_dot        = &__generate_dot_grafod,
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
};