#include "figura.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum TipoFigura { CIRCULO, RETANGULO };

struct Figura {
  float x, y;
  char *cor, *cor_borda;

  enum TipoFigura tipo;

  union {
    /** DADOS CIRCULO */
    struct {
      float r;
    } dados_circ;

    /** DADOS RETANGULO */
    struct {
      float h, w;
    } dados_rect;
  };
};

Figura cria_retangulo(float x, float y, float w, float h, char *cor,
                      char *cor_borda) {
  struct Figura *this = (struct Figura *) malloc(sizeof(struct Figura));

  this->tipo         = RETANGULO;
  this->x            = x;
  this->y            = y;
  this->dados_rect.w = w;
  this->dados_rect.h = h;
  this->cor          = (char *) malloc((strlen(cor) + 1) * sizeof(char));
  this->cor_borda    = (char *) malloc((strlen(cor_borda) + 1) * sizeof(char));

  strcpy(this->cor, cor);
  strcpy(this->cor_borda, cor_borda);

  return (void *) this;
}

Figura cria_circulo(float x, float y, float r, char *cor, char *cor_borda) {
  struct Figura *this = (struct Figura *) malloc(sizeof(struct Figura));

  this->tipo         = CIRCULO;
  this->x            = x;
  this->y            = y;
  this->dados_circ.r = r;
  this->cor          = (char *) malloc((strlen(cor) + 1) * sizeof(char));
  this->cor_borda    = (char *) malloc((strlen(cor_borda) + 1) * sizeof(char));

  strcpy(this->cor, cor);
  strcpy(this->cor_borda, cor_borda);

  return (void *) this;
}

int intercepta_figura(Figura f, Figura f2) {
  return 1;
}

int contem_ponto(Figura f, double x, double y) {
  return 1;
}

void print_figura(Figura f) {
  struct Figura *this = (struct Figura *) f;

  switch (this->tipo) {
    case RETANGULO:
      printf("Ret: (%.0f, %.0f) -> h: %.0f, w: %.0f\n", this->x, this->y,
             this->dados_rect.h, this->dados_rect.w);
      break;

    case CIRCULO:
      printf("Cir: (%.0f, %.0f) -> r: %.0f\n", this->x, this->y,
             this->dados_circ.r);
      break;
  }
}