#include "svg_linha.h"

#include <utils.h>
#include <stdlib.h>

#define SVG_FIGURA_SETA "<svg viewBox=\"0 0 451.846 451.847\" style=\"enable-background:new 0 0 451.846 451.847;\" xml:space=\"preserve\"><g transform=\"rotate(%.1f 225.923 225.923)\"><path d=\"M345.441,248.292L151.154,442.573c-12.359,12.365-32.397,12.365-44.75,0c-12.354-12.354-12.354-32.391,0-44.744		L278.318,225.92L106.409,54.017c-12.354-12.359-12.354-32.394,0-44.748c12.354-12.359,32.391-12.359,44.75,0l194.287,194.284 c6.177,6.18,9.262,14.271,9.262,22.366C354.708,234.018,351.617,242.115,345.441,248.292z\"/></g></svg>"
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

  char *seta;

  if (this->seta) {
    double angulo = rad_to_deg(Ponto2D_t.angle(Ponto2D_t.sub(b, a)));

    char *svg_seta = format_string(SVG_FIGURA_SETA, angulo);

    seta = format_string(
      "<svg style=\"fill:%s;opacity:0.8;\" width=\"%.2f\" height=\"%.2f\" x=\"%.2f\" y=\"%.2f\">\n%s</svg>",
      this->cor,
      20.0, 20.0,
      pSeta.x, pSeta.y, svg_seta
    );

    free(svg_seta);
  } else {
    seta = format_string("");
  }
  
  char *linha = format_string(
    "<line x1=\"%.1f\" y1=\"%.1f\" x2=\"%.1f\" y2=\"%.1f\" "
    "style=\"stroke:%s;stroke-width:4;opacity:0.8\" %s/>",
    a.x, a.y,
    b.x, b.y,
    this->cor,
    (this->tracejada) ? DASHED_STRING : ""
  );

  char *saida = format_string(
    "%s\n%s\n", linha, seta);

  free(linha);
  free(seta);

  return saida;
}

void free_svg_pontos(void *_this) {
  Seta this = _this;
  free(this->cor);
  free(this);
}