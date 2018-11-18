#include "dijkstra.h"

#include <stdlib.h>
#include <string.h>
#include <float.h>

#include <model/modules/hash.h>
#include <model/modules/logger.h>

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
 * Retorna o Posic do DVInfo de menor distancia na lista vertices
 */
static void maisProximoAux(void *_info, void *_pResult) {
  DVInfo info = (DVInfo) _info;
  DVInfo *pResult = (DVInfo *) _pResult;
  DVInfo result = *pResult;

  if (result == NULL) {
    *pResult = info;
  } else if (info->distancia < result->distancia) {
    *pResult = info;
  }
  
}

static DVInfo pegarMaisProximo(HashTable vertices) {
  DVInfo info = NULL;

  HashTable_t.map(vertices, &info, maisProximoAux);

  return info;
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

  HashTable naoVisitados = HashTable_t.create(HashTable_t.length(vertices));
  HashTable visitados    = HashTable_t.create(HashTable_t.length(vertices));

  // Insere o inicial no naoVisitados
  DVInfo inicial = pegarMaisProximo(vertices);
  HashTable_t.insert(naoVisitados, inicial->label, inicial);

  // Salva o target separadamente
  DVInfo target = HashTable_t.get(vertices, destino);

  // Enquanto todos nao forem visitados
  while (HashTable_t.length(naoVisitados) > 0) {

    // Pegue o nao visitado mais proximo
    DVInfo maisProximo = pegarMaisProximo(naoVisitados);

    // Tira do naoVisitados e coloca no visitados
    HashTable_t.remove(naoVisitados, maisProximo->label);
    HashTable_t.insert(visitados, maisProximo->label, maisProximo);

    // Se cheguei ao alvo para o loop
    if (!strcmp(destino, maisProximo->label)) {
      break;
    }

    // Passar por todos os adjacentes atualizando a distancia e o anterior
    Lista adjacentes = GrafoD_t.adjacentes(grafo, maisProximo->label);

    for (Posic it = Lista_t.get_first(adjacentes); it != NULL; it = Lista_t.get_next(adjacentes, it)) {
      char *adjacenteLabel  = Lista_t.get(adjacentes, it);

      // Se já foi visitado, nao tem porque testar
      if (HashTable_t.exists(visitados, adjacenteLabel)) {
        continue;
      }

      // Pega o DVInfo do vertice atual do loop
      DVInfo infoAdjacente = HashTable_t.get(vertices, adjacenteLabel);

      // Se ainda nao esta marcado para ser visitado, marcar
      if (!HashTable_t.exists(naoVisitados, adjacenteLabel)) {
        HashTable_t.insert(naoVisitados, infoAdjacente->label, infoAdjacente);
      }

      // Pega a informacao da aresta entre o mais proximo e o atual do loop
      void *infoAresta = GrafoD_t.get_info_aresta(grafo, maisProximo->label, adjacenteLabel);

      // Nova distancia a ser usada
      double novaDist = maisProximo->distancia + get_dist_aresta(infoAresta);

      // Se a nova distancia for menor do que a distancia atual, substitui
      if (novaDist < infoAdjacente->distancia) {
        infoAdjacente->distancia = novaDist;
        infoAdjacente->anterior  = maisProximo;
      }

    }

    Lista_t.destruir(adjacentes, 0);
  }

  // Gera o caminho com base no target
  Lista caminho = gerarCaminho(target);

  // Destrir auxiliares
  HashTable_t.destroy(visitados, NULL, 0);
  HashTable_t.destroy(naoVisitados, NULL, 0);
  HashTable_t.destroy(vertices, free, 0);

  return caminho;
}