#include "dijkstra.h"

#include <stdlib.h>
#include <string.h>
#include <float.h>

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
static Posic pegarMaisProximo(Lista vertices) {
  Posic it = Lista_t.get_first(vertices);

  Posic pMenor = it;
  DVInfo menor = Lista_t.get(vertices, pMenor);

  while (it) {
    DVInfo atual = Lista_t.get(vertices, it);

    if (atual->distancia < menor->distancia) {
      pMenor = it;
      menor = atual;
    }
    
    it = Lista_t.get_next(vertices, it);
  }

  return pMenor;
}

/**
 * Funcao para criar a lista inicial de DVInfos
 * Recebe um vetor com todos os labels do grafo
 * e o label do vertice inicial, para setar a distancia como 0
 */
static Lista getVertices(char **labels, int tamanho, char *inicial) {
  Lista vertices = Lista_t.create();

  for (int i = 0; i < tamanho; i++) {
    char *label = labels[i];

    DVInfo info = create_dvinfo(label, DBL_MAX);

    if (!strcmp(label, inicial))
      info->distancia = 0;

    Lista_t.insert(vertices, info);
  }

  return vertices;
}

/**
 * Funcao auxiliar para encontrar o DVInfo com o mesmo label passado
 */
static int labelIgual(const Item _info, const void *_label) {
  DVInfo info = (DVInfo) _info;
  char *label = (char *) _label;

  return strcmp(info->label, label);
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
  Lista lista = Lista_t.create();

  gerarCaminhoRec(target, lista);

  return lista;
}

/**
 * Funcao do Dijkstra em si. Baseado no algoritmo encontrado no site do Geeks for Geeks
 */
Lista dijkstra(GrafoD grafo, char *origem, char *destino, double (*get_dist_aresta)(void *aresta_info)) {

  char **labels = GrafoD_t.get_all_vertices(grafo);
  Lista vertices = getVertices(labels, GrafoD_t.total_vertices(grafo), origem);
  free(labels);

  Lista nao_visitados = Lista_t.copy(vertices);

  DVInfo target;

  // Enquanto todos nao forem visitados
  while (Lista_t.length(nao_visitados) > 0) {

    // Pegue o nao visitado mais proximo
    Posic pMaisProximo = pegarMaisProximo(nao_visitados);
    DVInfo maisProximo = Lista_t.get(nao_visitados, pMaisProximo);

    // Insira ele a lista de visitados
    Lista_t.remove(nao_visitados, pMaisProximo);

    // Se cheguei ao alvo para o loop
    if (!strcmp(destino, maisProximo->label)) {
      target = maisProximo;
      break;
    }

    // Passar por todos os adjacentes atualizando a distancia e o anterior
    Lista adjacentes = GrafoD_t.adjacentes(grafo, maisProximo->label);

    Posic it = Lista_t.get_first(adjacentes);

    while (it) {
      char *adjacenteLabel  = Lista_t.get(adjacentes, it);

      // Pega o DVInfo do vertice atual do loop
      Posic pInfoAtual = Lista_t.search(vertices, Lista_t.get_first(vertices), adjacenteLabel, labelIgual);
      DVInfo infoAtual = Lista_t.get(vertices, pInfoAtual);
      
      // Pega a informacao da aresta entre o mais proximo e o atual do loop
      void *infoAresta = GrafoD_t.get_info_aresta(grafo, maisProximo->label, adjacenteLabel);

      // Nova distancia a ser usada
      double novaDist = maisProximo->distancia + get_dist_aresta(infoAresta);

      // Se a nova distancia for menor do que a distancia atual, substitui
      if (novaDist < infoAtual->distancia) {
        infoAtual->distancia = novaDist;
        infoAtual->anterior  = maisProximo;
      }

      it = Lista_t.get_next(adjacentes, it);
    }

    Lista_t.destruir(adjacentes, 0);
  }

  Lista_t.destruir(nao_visitados, 0);

  Lista caminho = gerarCaminho(target);

  Lista_t.destruir(vertices, free);

  return caminho;
}