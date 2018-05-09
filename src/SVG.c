#include "SVG.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arquivo.h"
#include "utils.h"

struct SVG {
  Arquivo saida;
};

SVG cria_SVG(char *path, float max_width, float max_height) {
  struct SVG *this;
  char *saida;
  size_t length;

  this = (struct SVG *) malloc(sizeof(struct SVG));

  length = 12 + 60;

  saida = (char *) calloc(length, sizeof(char));

  this->saida = abrir_arquivo(path, ESCRITA);

  sprintf(
    saida,
    "<svg width=\"%4.1f\" height=\"%4.1f\" "
    "xmlns=\"http://www.w3.org/2000/svg\">",
    max_width,
    max_height);

  escrever_linha(this->saida, saida);

  free(saida);

  return (SVG) this;
}

void desenha_figura(SVG s, Figura f) {
  struct SVG *this;

  float x, y, r, h, w;
  char *cor, *cor_borda, *linha;
  size_t length;

  this = (struct SVG *) s;

  x         = get_x(f);
  y         = get_y(f);
  cor       = get_cor(f);
  cor_borda = get_cor_borda(f);

  length = strlen(cor) + strlen(cor_borda) + 12;

  switch (get_tipo_figura(f)) {
    case CIRCULO:
      length += 77;

      r = get_r(f);
      length += 6;

      linha = (char *) malloc(length * sizeof(char));

      sprintf(
        linha,
        "<circle cx=\"%4.1f\" cy=\"%4.1f\" r=\"%4.1f\" "
        "style=\"fill:%s;stroke-width:2;stroke:%s;opacity:0.4\" />",
        x,
        y,
        r,
        cor,
        cor_borda);

      escrever_linha(this->saida, linha);
      break;

    case RETANGULO:
      length += 87;

      h = get_h(f);
      w = get_w(f);

      length += 12;

      linha = (char *) malloc(length * sizeof(char));

      sprintf(
        linha,
        "<rect x=\"%4.1f\" y=\"%4.1f\" width=\"%4.1f\" height=\"%4.1f\" "
        "style=\"fill:%s;stroke-width:2;stroke:%s;opacity:0.4\" />",
        x,
        y,
        w,
        h,
        cor,
        cor_borda);

      escrever_linha(this->saida, linha);
      break;
  }

  free(linha);
}

void desenha_dashed_rect(SVG s, Figura f) {
  struct SVG *this;

  float x, y, h, w;
  char *linha;
  size_t length;

  if (get_tipo_figura(f) != RETANGULO)
    return;

  this = (struct SVG *) s;

  x = get_x(f);
  y = get_y(f);
  h = get_h(f);
  w = get_w(f);

  length = 115 + 24;

  linha = (char *) malloc(length * sizeof(char));

  sprintf(
    linha,
    "<rect width=\"%4.1f\" height=\"%4.1f\" x=\"%4.1f\" y=\"%4.1f\" "
    "stroke-dasharray=\"3, "
    "3\" style=\"fill:transparent;stroke-width:2;stroke:purple\"/>",
    w,
    h,
    x,
    y);

  escrever_linha(this->saida, linha);

  free(linha);
}

void escreve_texto(
  SVG s, char *texto, float x, float y, float tamanho, char *cor) {
  struct SVG *this;

  size_t length;
  char *linha;

  this = (struct SVG *) s;

  length = 74 + 18;
  length += strlen(texto);
  length += strlen(cor);

  linha = (char *) malloc(length * sizeof(char));

  sprintf(
    linha,
    "<text x=\"%4.1f\" y=\"%4.1f\" "
    "style=\"fill:%s;font-size:%4.1fpx;font-family:sans-serif\">%s</text>",
    x,
    y,
    cor,
    tamanho,
    texto);

  escrever_linha(this->saida, linha);

  free(linha);
}

void desenha_linha(SVG s, float x1, float y1, float x2, float y2, char *cor) {
  struct SVG *this;

  int padrao;
  size_t length;
  char *linha;

  this   = (struct SVG *) s;
  padrao = 0;

  /* Cor padrão */
  if (!cor) {
    cor = (char *) malloc(6 * sizeof(char));
    strcpy(cor, "black");
    padrao = 1;
  }

  length = 64 + 24;
  length += strlen(cor);

  linha = (char *) malloc(length * sizeof(char));

  sprintf(
    linha,
    "<line x1=\"%4.1f\" y1=\"%4.1f\" x2=\"%4.1f\" y2=\"%4.1f\" "
    "style=\"stroke:%s;stroke-width:1\" />",
    x1,
    y1,
    x2,
    y2,
    cor);

  escrever_linha(this->saida, linha);

  free(linha);
  if (padrao)
    free(cor);
}

void salva_SVG(SVG s) {
  struct SVG *this;
  this = (struct SVG *) s;
  escrever_linha(this->saida, "</svg>");

  fechar_arquivo(this->saida);
}

void destruir_SVG(SVG s) {
  free(s);
}