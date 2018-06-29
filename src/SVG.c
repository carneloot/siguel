#include "SVG.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arquivo.h"
#include "utils.h"

#include "modules/logger.h"

#define DASHED_STRING "stroke-dasharray=\"5, 5\" "

struct SVG {
  Arquivo saida;
};

SVG cria_SVG(char *path, float max_width, float max_height) {
  struct SVG *this;

  this = (struct SVG *) malloc(sizeof(struct SVG));

  this->saida = abrir_arquivo(path, ESCRITA);

  escrever_linha(
    this->saida,
    "<svg width=\"%.1f\" height=\"%.1f\" "
    "xmlns=\"http://www.w3.org/2000/svg\">\n",
    max_width,
    max_height);

  return (SVG) this;
}

void desenha_figura(SVG s, Figura f, float opacity, int is_dashed) {
  struct SVG *this;

  float x, y, r, h, w;
  char *cor, *cor_borda;

  this = (struct SVG *) s;

  x         = get_x(f);
  y         = get_y(f);
  r         = get_r(f);
  w         = get_w(f);
  h         = get_h(f);
  cor       = get_cor(f);
  cor_borda = get_cor_borda(f);

  switch (get_tipo_figura(f)) {
    case CIRCULO:
      escrever_linha(
        this->saida,
        "<circle cx=\"%.1f\" cy=\"%.1f\" r=\"%.1f\" "
        "style=\"fill:%s;stroke-width:2;stroke:%s;opacity:%.2f\" %s/>\n",
        x,
        y,
        r,
        cor,
        cor_borda,
        opacity,
        (is_dashed) ? DASHED_STRING : "");
      break;

    case RETANGULO:
      escrever_linha(
        this->saida,
        "<rect x=\"%.1f\" y=\"%.1f\" width=\"%.1f\" height=\"%.1f\" "
        "style=\"fill:%s;stroke-width:2;stroke:%s;opacity:%.2f\" %s/>\n",
        x,
        y,
        w,
        h,
        cor,
        cor_borda,
        opacity,
        (is_dashed) ? DASHED_STRING : "");
      break;
  }
}

void desenha_elemento(SVG this, Elemento e) {
  Figura figura = get_figura_elemento(e);

  desenha_figura(this, figura, 0.4, SVG_BORDA_SOLIDA);

  // Se for quadra, desenhar o CEP no meio
  if (get_tipo_elemento(e) == QUADRA) {
    float x, y;
    char *cep;

    x = get_x(e) + 5;
    y = get_y(e) + get_altura(e) - 5;

    cep = get_cep_elemento(e);

    escreve_texto(this, cep, x, y, 10, get_cor_borda(e));
  }

  destruir_figura(figura);

}

void escreve_texto(
  SVG s, char *texto, float x, float y, float tamanho, char *cor) {
  struct SVG *this;

  this = (struct SVG *) s;

  escrever_linha(
    this->saida,
    "<text x=\"%.1f\" y=\"%.1f\" "
    "style=\"fill:%s;font-size:%.1fpx;font-family:sans-serif\">%s</text>\n",
    x,
    y,
    cor,
    tamanho,
    texto);
}

void desenha_linha(SVG s, float x1, float y1, float x2, float y2, char *cor) {
  struct SVG *this;

  this = (struct SVG *) s;

  escrever_linha(
    this->saida,
    "<line x1=\"%.1f\" y1=\"%.1f\" x2=\"%.1f\" y2=\"%.1f\" "
    "style=\"stroke:%s;stroke-width:1\" />\n",
    x1,
    y1,
    x2,
    y2,
    (!!cor) ? cor : "black");
}

void salva_SVG(SVG s) {
  struct SVG *this;
  this = (struct SVG *) s;
  escrever_linha(this->saida, "</svg>\n");

  LOG_PRINT(LOG_FILE, "SVG \"%s\" salvo com sucesso.", get_path(this->saida));

  fechar_arquivo(this->saida);
}

void destruir_SVG(SVG s) {
  free(s);
}