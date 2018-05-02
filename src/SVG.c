#include "SVG.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arquivo.h"
#include "utils.h"

#define MAX_WIDTH 500
#define MAX_HEIGHT 500

struct SVG {
  Arquivo saida;
};

SVG cria_SVG(char *path) {
  struct SVG *this = (struct SVG *) malloc(sizeof(struct SVG));

  this->saida = abrir_arquivo(path, ESCRITA);

  // Codigo para colocar o MAX_WIDTH e o MAX_HEIGHT no inicio do SVG
  size_t length;
  char *linha;

  length = 25;
  length += num_digits(MAX_HEIGHT) - 2;
  length += num_digits(MAX_WIDTH) - 2;

  linha = (char *) malloc(length * sizeof(char));
  sprintf(linha, "<svg width=\"%d\" height=\"%d\">", MAX_WIDTH, MAX_HEIGHT);

  escrever_linha(this->saida, linha);

  free(linha);

  return (void *) this;
}

void desenha_figura(SVG s, Figura f) {
  struct SVG *this = (struct SVG *) s;

  float x, y, r, h, w;
  char *cor, *cor_borda, *linha;
  size_t length;

  x         = get_x(f);
  y         = get_y(f);
  cor       = get_cor(f);
  cor_borda = get_cor_borda(f);

  length = strlen(cor) + strlen(cor_borda);
  length += num_digits(x);
  length += num_digits(y);

  switch (get_tipo_figura(f)) {
    case CIRCULO:
      length += 65;

      r = get_r(f);
      length += num_digits(r);

      linha = (char *) malloc(length * sizeof(char));

      sprintf(linha,
              "<circle cx=\"%.1f\" cy=\"%.1f\" r=\"%.1f\" "
              "style=\"fill:%s;stroke-width:4;stroke:%s\" />",
              x,
              y,
              r,
              cor,
              cor_borda);

      escrever_linha(this->saida, linha);
      break;

    case RETANGULO:
      length += 75;

      h = get_h(f);
      w = get_w(f);

      length += num_digits(h);
      length += num_digits(w);

      linha = (char *) malloc(length * sizeof(char));

      sprintf(linha,
              "<rect x=\"%.1f\" y=\"%.1f\" width=\"%.1f\" height=\"%.1f\" "
              "style=\"fill:%s;stroke-width:4;stroke:%s\" />",
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

void escreve_texto(
    SVG s, char *texto, float x, float y, float tamanho, char *cor) {
  struct SVG *this = (struct SVG *) s;

  size_t length;
  char *linha;

  length = 74;
  length += num_digits(x);
  length += num_digits(y);
  length += num_digits(tamanho);
  length += strlen(texto);
  length += strlen(cor);

  linha = (char *) malloc(length * sizeof(char));

  sprintf(linha,
          "<text x=\"%.1f\" y=\"%.1f\" "
          "style=\"fill:%s;font-size:%.1fpx;font-family:sans-serif\">%s</text>",
          x,
          y,
          cor,
          tamanho,
          texto);

  escrever_linha(this->saida, linha);

  free(linha);
}

void desenha_linha(SVG s, float x1, float y1, float x2, float y2, char *cor) {
  struct SVG *this = (struct SVG *) s;

  int padrao = 0;
  size_t length;
  char *linha;

  // Cor padrão
  if (!cor) {
    cor = (char *) malloc(6 * sizeof(char));
    strcpy(cor, "black");
    padrao = 1;
  }

  length = 64;
  length += num_digits(x1);
  length += num_digits(y1);
  length += num_digits(x2);
  length += num_digits(y2);
  length += strlen(cor);

  linha = (char *) malloc(length * sizeof(char));

  sprintf(linha,
          "<line x1=\"%.1f\" y1=\"%.1f\" x2=\"%.1f\" y2=\"%.1f\" "
          "style=\"stroke:%s;stroke-width:3\" />",
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
  struct SVG *this = (struct SVG *) s;
  escrever_linha(this->saida, "</svg>");

  fechar_arquivo(this->saida);
}

void destruir_SVG(SVG s) {
  free(s);
}