#include "svg_linha.h"

#include <stdlib.h>

#include <model/utils.h>

#define DASHED_STRING "stroke-dasharray=\"5, 5\" "

Seta cria_svg_pontos(Ponto2D a, Ponto2D b, char *cor, int seta, int tracejada) {
  struct Seta *this = calloc(1, sizeof(*this));

  this->a    = a;
  this->b    = b;
  this->cor  = format_string(cor);
  this->seta = seta;
  this->tracejada = tracejada;

  return this;
}

char *svg_pontos(void *_this) {
  struct Seta *this = _this;

  Ponto2D a = this->a;
  Ponto2D b = this->b;

  Ponto2D pSeta = Ponto2D_t.add(a, b);
  pSeta = Ponto2D_t.mult(pSeta, 0.5);
  pSeta = Ponto2D_t.sub_scalar(pSeta, 10);

  char *saida = format_string(
    "<line %s x1=\"%.1f\" y1=\"%.1f\" x2=\"%.1f\" y2=\"%.1f\" "
    "style=\"stroke:%s;stroke-width:4;opacity:0.8\" %s/>",
    (this->seta) ? "marker-end='url(#arrowhead)'" : "",
    a.x, a.y,
    b.x, b.y,
    this->cor,
    (this->tracejada) ? DASHED_STRING : ""
  );

  return saida;
}

void free_svg_pontos(void *_this) {
  Seta this = _this;
  free(this->cor);
  free(this);
}