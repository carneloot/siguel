#ifndef __CONTROLADOR_R__
#define __CONTROLADOR_R__

#include <modules/lista.h>
#include <modules/kdtree.h>
#include <modules/ponto2d.h>

struct Controlador {
  Lista saida;
  Lista saida_svg_qry;

  char *nome_base;
  char *dir_saida;
  char *dir_entrada;
  char *arq_query;

  Lista figuras;

  int linha_atual;

  Lista sobreposicoes;

  KDTree elementos[4];
  char *cores[4];
  char *cores_borda[4];

  Ponto2D max_geo, max_qry;

  Lista fila_execucao;
};

extern void desenhar_todas_figuras(void *c, void *svg);

extern void desenhar_sobreposicoes(void *c, void *svg);

extern void desenhar_elementos(void *this, void *svg);

#endif /* __CONTROLADOR_R__ */