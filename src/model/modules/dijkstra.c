#include "dijkstra.h"

#include <stdlib.h>
#include <string.h>
#include <float.h>

#include <model/modules/hash.h>
#include <model/modules/logger.h>
#include <model/modules/pqueue.h>

/**
 * Struct feita para guardar as informacoes do vertice
 * [D]ijkstra [V]ertice [Info]
 */
struct DVInfo {
  char *label;             // Label usado para saber qual vertice e
  double distancia;        // Distancia do inicial ate o vertice
  struct DVInfo *anterior; // Vertice anterior para criar o caminho
};

typedef struct DVInfo *DVInfo;

/** Auxiliar para criar o DVInfo */
static DVInfo create_dvinfo(char *label, double distancia) {
  DVInfo this = calloc(1, sizeof(*this));

  this->label     = label;
  this->distancia = distancia;
  this->anterior  = NULL;

  return this;
}

/**
 * Funcao recursiva para gerar a lista do caminho
 * mais proximo.
 */
static void gerarCaminhoRec(DVInfo atual, Lista lista) {
  if (atual->anterior == NULL) {
    Lista_t.insert(lista, atual->label);
    return;
  }

  gerarCaminhoRec(atual->anterior, lista);

  Lista_t.insert(lista, atual->label);
}

/**
 * Funcao para gerar o caminho mais proximo.
 * Recebe o DVInfo do destino.
 */
static Lista gerarCaminho(DVInfo target) {

  if (target->anterior == NULL)
    return NULL;
    
  Lista lista = Lista_t.create();

  gerarCaminhoRec(target, lista);

  return lista;
}

/**
 * Funcao do Dijkstra em si. Baseado no algoritmo encontrado no site do Geeks for Geeks
 */
Lista dijkstra(GrafoD grafo, char *origem, char *destino, double (*get_dist_aresta)(void *aresta_info)) {
  LOG_PRINT(LOG_FILE, "Dijkstra: gerando caminho de \"%s\" ate \"%s\".", origem, destino);

  HashTable vertices = HashTable_t.create(GrafoD_t.total_vertices(grafo));

  PQueue fila = pq_create(HashTable_t.max_size(vertices));

  DVInfo inicial = create_dvinfo(origem, 0);
  HashTable_t.insert(vertices, inicial->label, inicial);

  DVInfo target = create_dvinfo(destino, DBL_MAX);
  HashTable_t.insert(vertices, target->label, target);

  pq_insert(fila, inicial->distancia, inicial);

  // Enquanto a fila nao estiver vazia
  while (pq_getsize(fila) > 0) {
    DVInfo menorDistancia = pq_extractmin(fila);

    if (menorDistancia == target)
      break;

    Lista adjacentes = GrafoD_t.adjacentes(grafo, menorDistancia->label);

    for (Posic it = Lista_t.get_first(adjacentes); it != NULL; it = Lista_t.get_next(adjacentes, it)) {
      char *labelAdjacente = Lista_t.get(adjacentes, it);

      DVInfo adjacente;

      if (HashTable_t.exists(vertices, labelAdjacente)) {
        adjacente = HashTable_t.get(vertices, labelAdjacente);
      } else {
        adjacente = create_dvinfo(labelAdjacente, DBL_MAX);
        HashTable_t.insert(vertices, adjacente->label, adjacente);
      }

      void *infoAresta = GrafoD_t.get_info_aresta(grafo, menorDistancia->label, labelAdjacente);

      double newDist = menorDistancia->distancia + get_dist_aresta(infoAresta);

      if (newDist < adjacente->distancia) {
        adjacente->distancia = newDist;
        adjacente->anterior = menorDistancia;
        pq_insert(fila, adjacente->distancia, adjacente);
      }
    }

    Lista_t.destruir(adjacentes, NULL);

  }

  pq_destroy(fila, NULL);

  Lista caminho = gerarCaminho(target);

  HashTable_t.destroy(vertices, free, 0);

  return caminho;
}