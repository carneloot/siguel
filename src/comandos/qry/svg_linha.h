#ifndef __SVG_LINHA_H__
#define __SVG_LINHA_H__

#include <modules/ponto2d.h>

struct Seta {
  Ponto2D a, b;
  char *cor;
  int seta;
};

typedef struct Seta *Seta;

Seta cria_svg_pontos(Ponto2D a, Ponto2D b, char *cor, int seta);

char *svg_pontos(void *_pontos);

void free_svg_pontos(void * this);

#endif /* __SVG_LINHA_H__ */