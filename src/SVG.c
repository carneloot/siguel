#include "SVG.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arquivo.h"
#include "utils.h"

#define DASHED_STRING "stroke-dasharray=\"5, 5\" "

struct SVG {
  Arquivo saida;
};

SVG cria_SVG(char *path, double max_width, double max_height) {
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

  double x, y, r, h, w;
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
    Ponto2D pos;
    char *cep;

    pos = get_pos(e);
    pos.x += 5;
    pos.y += get_altura(e) - 5;

    cep = get_cep_elemento(e);

    escreve_texto(this, cep, pos, 10, get_cor_borda(e));
  }

  destruir_figura(figura);
}

void desenha_asset(SVG _this, Ponto2D pos, Ponto2D size, char *path) {
  struct SVG *this = (struct SVG *) _this;

  Arquivo arq_asset  = abrir_arquivo(path, LEITURA);
  char *asset_string = ler_arquivo_inteiro(arq_asset);
  fechar_arquivo(arq_asset);

  escrever_linha(
    this->saida,
    "<svg width=\"%.2lf\" height=\"%.2lf\" x=\"%.2lf\" y=\"%.2lf\">\n%s</svg>\n",
    size.x,
    size.y,
    pos.x,
    pos.y,
    asset_string);

  free(asset_string);
}

void escreve_texto(SVG s, char *texto, Ponto2D pos, float tamanho, char *cor) {
  struct SVG *this;

  this = (struct SVG *) s;

  escrever_linha(
    this->saida,
    "<text x=\"%.1f\" y=\"%.1f\" "
    "style=\"fill:%s;font-size:%.1fpx;font-family:sans-serif\">%s</text>\n",
    pos.x,
    pos.y,
    cor,
    tamanho,
    texto);
}

void desenha_linha(SVG s, Ponto2D a, Ponto2D b, char *cor) {
  struct SVG *this;

  this = (struct SVG *) s;

  escrever_linha(
    this->saida,
    "<line x1=\"%.1f\" y1=\"%.1f\" x2=\"%.1f\" y2=\"%.1f\" "
    "style=\"stroke:%s;stroke-width:1\" />\n",
    a.x,
    a.y,
    b.x,
    b.y,
    (!!cor) ? cor : "black");
}

void salva_SVG(SVG s) {
  struct SVG *this;
  this = (struct SVG *) s;
  escrever_linha(this->saida, "</svg>\n");

  fechar_arquivo(this->saida);
}

void destruir_SVG(SVG s) {
  free(s);
}