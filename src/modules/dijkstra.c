#include "dijkstra.h"

#include <stdlib.h>
#include <string.h>
#include <float.h>

#include <modules/hash.h>
#include <modules/logger.h>
#include <modules/pqueue.h>

/**
 * Struct feita para guardar as informacoes do vertice
 * [D]ijkstra [V]ertice [Info]
 */
struct DVInfo {
  char *label;             // Label usado para saber qual vertice e
  double distancia;        // Distancia do inicial ate o vertice
  double heuristica;       // Valor de heuristica para o A*
  struct DVInfo *anterior; // Vertice anterior para criar o caminho
};

typedef struct DVInfo *DVInfo;

/** Auxiliar para criar o DVInfo */
static DVInfo create_dvinfo(char *label, double distancia, double heuristica) {
  DVInfo this = calloc(1, sizeof(*this));

  this->label      = label;
  this->distancia  = distancia;
  this->heuristica = heuristica;
  this->anterior   = NULL;

  return this;
}

/**
 * Funcao recursiva para gerar a lista do caminho
 * mais proximo.
 */
static void gerarCaminhoRec(DVInfo atual, Lista_t lista) {
  if (atual->anterior == NULL) {
    lt_insert(lista, atual->label);
    return;
  }

  gerarCaminhoRec(atual->anterior, lista);

  lt_insert(lista, atual->label);
}

/**
 * Funcao para gerar o caminho mais proximo.
 * Recebe o DVInfo do destino.
 */
static Lista_t gerarCaminho(DVInfo target) {

  if (target->anterior == NULL)
    return NULL;
    
  Lista_t lista = lt_create();

  gerarCaminhoRec(target, lista);

  return lista;
}

/**
 * Funcao do A* em si. Baseado no algoritmo encontrado no site do Geeks for Geeks
 */
Lista_t astar(
  GrafoD grafo,
  char *origem, char *destino,
  double (*get_dist_aresta)(void *aresta_info),
  double (*get_heuristica)(void *atual, void *destino)) {
  LOG_PRINT(LOG_FILE, "A*: gerando caminho de \"%s\" ate \"%s\".", origem, destino);

  HashTable_t vertices = ht_create(GrafoD_t.total_vertices(grafo));

  PQueue_t fila = pq_create(ht_max_size(vertices));

  void *info_inicial = GrafoD_t.get_info_vertice(grafo, origem);
  void *info_target  = GrafoD_t.get_info_vertice(grafo, destino);

  DVInfo inicial = create_dvinfo(origem, 0, get_heuristica(info_inicial, info_target));
  ht_insert(vertices, inicial->label, inicial);

  DVInfo target = create_dvinfo(destino, DBL_MAX, 0);
  ht_insert(vertices, target->label, target);

  pq_insert(fila, inicial->distancia + inicial->heuristica, inicial);

  // Enquanto a fila nao estiver vazia
  while (pq_getsize(fila) > 0) {
    DVInfo menorDistancia = pq_extractmin(fila);

    if (menorDistancia == target)
      break;

    Lista_t adjacentes = GrafoD_t.adjacentes(grafo, menorDistancia->label);

    for (Posic_t it = lt_get_first(adjacentes); it != NULL; it = lt_get_next(adjacentes, it)) {
      char *labelAdjacente = lt_get(adjacentes, it);

      DVInfo adjacente;

      if (ht_exists(vertices, labelAdjacente)) {
        adjacente = ht_get(vertices, labelAdjacente);
      } else {
        void *info_adjacente = GrafoD_t.get_info_vertice(grafo, labelAdjacente);
        adjacente = create_dvinfo(labelAdjacente, DBL_MAX, get_heuristica(info_adjacente, info_target));
        ht_insert(vertices, adjacente->label, adjacente);
      }

      void *infoAresta = GrafoD_t.get_info_aresta(grafo, menorDistancia->label, labelAdjacente);

      double newDist = menorDistancia->distancia + get_dist_aresta(infoAresta);

      if (newDist < adjacente->distancia) {
        adjacente->distancia = newDist;
        adjacente->anterior  = menorDistancia;
        pq_insert(fila, adjacente->distancia + adjacente->heuristica, adjacente);
      }
    }

    lt_destroy(adjacentes, NULL);

  }

  pq_destroy(fila, NULL);

  Lista_t caminho = gerarCaminho(target);

  ht_destroy(vertices, free, 0);

  return caminho;
}

static double return_zero(void *atual, void *target) {
  return 0.0;
}

Lista_t dijkstra(
  GrafoD grafo,
  char *origem, char *destino,
  double (*get_dist_aresta)(void *aresta_info)) {
  return astar(grafo, origem, destino, get_dist_aresta, return_zero);
}