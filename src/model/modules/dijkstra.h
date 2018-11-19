#ifndef __DIJKSTRA_H__
#define __DIJKSTRA_H__

#include "lista.h"
#include "grafod.h"

/**
 * Essa funcao encontra o melhor caminho de origem até destino pelo grafo passasdo.
 * Nao existe tratamento de erros, ou seja, se um vertice passado nao existir, vai bugar tudo.
 * 
 * A funcao get_dist_aresta recebe a informacao atribuida a aresta
 * e retorna a distancia utilizada para o calculo do algoritmo
 */
Lista dijkstra(GrafoD grafo, char *origem, char *destino, double (*get_dist_aresta)(void *aresta_info));

#endif /* __DIJKSTRA_H__ */