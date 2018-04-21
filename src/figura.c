#include "figura.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

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

Figura cria_retangulo(
    float x, float y, float w, float h, char *cor, char *cor_borda) {
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
  struct Figura *this  = (struct Figura *) f;
  struct Figura *other = (struct Figura *) f2;

  float dist_squared;

  if (this->tipo == CIRCULO && other->tipo == CIRCULO) {
    dist_squared = pow(this->x - other->x, 2) + pow(this->y - other->y, 2);
    return (dist_squared <= pow(this->dados_circ.r + other->dados_circ.r, 2));
  }

  if (this->tipo == RETANGULO && other->tipo == RETANGULO) {
    return !(this->x >= other->x + other->dados_rect.w ||
             this->x + this->dados_rect.w <= other->x ||
             this->y >= other->y + other->dados_rect.h ||
             this->y + this->dados_rect.h <= other->y);
  }

  struct Figura *circ, *rect;
  float x_perto, y_perto;

  if (this->tipo == CIRCULO) {
    circ = this;
    rect = other;
  } else {
    circ = other;
    rect = this;
  }

  x_perto = max(rect->x, min(circ->x, rect->x + rect->dados_rect.w));
  y_perto = max(rect->y, min(circ->y, rect->y + rect->dados_rect.h));

  return (pow(circ->x - x_perto, 2) + pow(circ->y - y_perto, 2) <
          pow(circ->dados_circ.r, 2));
}

int contem_ponto(Figura f, double x, double y) {
  struct Figura *this = (struct Figura *) f;

  if (this->tipo == CIRCULO) {
    float dist_squared = pow(this->x - x, 2) + pow(this->y - y, 2);

    return (dist_squared <= pow(this->dados_circ.r, 2));
  }

  if (this->tipo == RETANGULO) {
    return !(x < this->x || x > this->x + this->dados_rect.w || y < this->y ||
             y > this->y + this->dados_rect.h);
  }
}

void print_figura(Figura f) {
  struct Figura *this = (struct Figura *) f;

  switch (this->tipo) {
    case RETANGULO:
      printf("Ret: (%.0f, %.0f) -> h: %.0f, w: %.0f\n",
             this->x,
             this->y,
             this->dados_rect.h,
             this->dados_rect.w);
      break;

    case CIRCULO:
      printf("Cir: (%.0f, %.0f) -> r: %.0f\n",
             this->x,
             this->y,
             this->dados_circ.r);
      break;
  }
}