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
 * Funcao para criar a lista inicial de DVInfos
 * Recebe um vetor com todos os labels do grafo
 * e o label do vertice inicial, para setar a distancia como 0
 */
static HashTable getVertices(char **labels, int tamanho, char *inicial) {
  int tamanho_hash = (int) (tamanho / 0.7) + 1;
  HashTable vertices = HashTable_t.create(tamanho_hash);

  for (int i = 0; i < tamanho; i++) {
    char *label = labels[i];

    DVInfo info = create_dvinfo(label, DBL_MAX);

    if (!strcmp(label, inicial))
      info->distancia = 0;

    HashTable_t.insert(vertices, info->label, info);
  }

  return vertices;
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
  char **labels = GrafoD_t.get_all_vertices(grafo);
  HashTable vertices = getVertices(labels, GrafoD_t.total_vertices(grafo), origem);
  free(labels);
  
  LOG_PRINT(LOG_FILE, "Dijkstra: gerando caminho de \"%s\" ate \"%s\".", origem, destino);

  PQueue fila = pq_create(HashTable_t.length(vertices));

  DVInfo inicial = HashTable_t.get(vertices, origem);
  DVInfo target  = HashTable_t.get(vertices, destino);

  pq_insert(fila, inicial->distancia, inicial);

  // Enquanto a fila nao estiver vazia
  while (pq_getsize(fila) > 0) {
    DVInfo menorDistancia = pq_extractmin(fila);

    if (menorDistancia == target)
      break;

    Lista adjacentes = GrafoD_t.adjacentes(grafo, menorDistancia->label);

    for (Posic it = Lista_t.get_first(adjacentes); it != NULL; it = Lista_t.get_next(adjacentes, it)) {
      char *labelAdjacente = Lista_t.get(adjacentes, it);
      
      DVInfo adjacente = HashTable_t.get(vertices, labelAdjacente);

      void *infoAresta = GrafoD_t.get_info_aresta(grafo, menorDistancia->label, labelAdjacente);

      double newDist = menorDistancia->distancia + get_dist_aresta(infoAresta);

      if (newDist < adjacente->distancia) {
        adjacente->distancia = newDist;
        adjacente->anterior = menorDistancia;
        pq_insert(fila, newDist, adjacente);
      }
    }

    Lista_t.destruir(adjacentes, NULL);

  }

  pq_destroy(fila, NULL);

  Lista caminho = gerarCaminho(target);

  HashTable_t.destroy(vertices, free, 0);

  return caminho;
}