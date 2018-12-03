#ifndef __SVG_LINHA_H__
#define __SVG_LINHA_H__

#include <modules/ponto2d.h>

struct Seta {
  Ponto2D_t a, b;
  char *cor;
  int seta;
  int tracejada;
};

typedef struct Seta *Seta;

Seta cria_svg_pontos(Ponto2D_t a, Ponto2D_t b, char *cor, int seta, int tracejada);

char *svg_pontos(void *_pontos);

void free_svg_pontos(void * this);

#endif /* __SVG_LINHA_H__ */