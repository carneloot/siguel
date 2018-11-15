#ifndef __DIJKSTRA_H__
#define __DIJKSTRA_H__

#include "lista.h"
#include "grafod.h"

Lista dijkstra(GrafoD grafo, char *origem, char *destino, double (*get_dist_aresta)(void *aresta_info));

#endif /* __DIJKSTRA_H__ */