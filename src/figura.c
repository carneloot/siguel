#include "figura.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

#define STROKE_SIZE 2

struct Figura {
  float x, y;
  char *cor, *cor_borda;

  enum TipoFigura tipo;

  /** DADOS DE CADA TIPO */
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

  trim(&this->cor);
  trim(&this->cor_borda);

  return (void *) this;
}

int intercepta_figura(Figura f, Figura f2) {
  struct Figura *this  = (struct Figura *) f;
  struct Figura *other = (struct Figura *) f2;

  if (this->tipo == CIRCULO && other->tipo == CIRCULO) {
    return (dist_squared(this->x, this->y, other->x, other->y) <
            pow(this->dados_circ.r + other->dados_circ.r, 2));
  }

  if (this->tipo == RETANGULO && other->tipo == RETANGULO) {
    return !(this->x > other->x + other->dados_rect.w ||
             this->x + this->dados_rect.w < other->x ||
             this->y > other->y + other->dados_rect.h ||
             this->y + this->dados_rect.h < other->y);
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

  x_perto = fmaxf(rect->x, fminf(circ->x, rect->x + rect->dados_rect.w));
  y_perto = fmaxf(rect->y, fminf(circ->y, rect->y + rect->dados_rect.h));

  return (dist_squared(circ->x, circ->y, x_perto, y_perto) <
          pow(circ->dados_circ.r, 2));
}

int contem_ponto(Figura f, double x, double y) {
  struct Figura *this = (struct Figura *) f;

  if (this->tipo == CIRCULO) {
    return (dist_squared(this->x, this->y, x, y) <= pow(this->dados_circ.r, 2));
  }

  if (this->tipo == RETANGULO) {
    return !(x < this->x || x > this->x + this->dados_rect.w || y < this->y ||
             y > this->y + this->dados_rect.h);
  }

  return 0;
}

void get_centro_massa(Figura f, float *x, float *y) {
  struct Figura *this = (struct Figura *) f;

  *x = this->x;
  *y = this->y;

  if (this->tipo == RETANGULO) {
    *x += this->dados_rect.w / 2;
    *y += this->dados_rect.h / 2;
  }
}

Figura get_rect_sobreposicao(Figura f1, Figura f2) {
  struct Figura *this  = (struct Figura *) f1;
  struct Figura *other = (struct Figura *) f2;

  float x, y, h, w;

  if (this->tipo == CIRCULO && other->tipo == CIRCULO) {
    x = fminf(this->x - this->dados_circ.r, other->x - other->dados_circ.r);
    y = fminf(this->y - this->dados_circ.r, other->y - other->dados_circ.r);

    w = fmaxf(this->x + this->dados_circ.r, other->x + other->dados_circ.r);
    h = fmaxf(this->y + this->dados_circ.r, other->y + other->dados_circ.r);
  }

  else if (this->tipo == RETANGULO && other->tipo == RETANGULO) {
    x = fminf(this->x, other->x);
    y = fminf(this->y, other->y);

    w = fmaxf(this->x + this->dados_rect.w, other->x + other->dados_rect.w);
    h = fmaxf(this->y + this->dados_rect.h, other->y + other->dados_rect.h);
  } else {
    struct Figura *circ, *rect;

    if (this->tipo == CIRCULO) {
      circ = this;
      rect = other;
    } else {
      circ = other;
      rect = this;
    }

    x = fminf(rect->x, circ->x - circ->dados_circ.r);
    y = fminf(rect->y, circ->y - circ->dados_circ.r);

    w = fmaxf(rect->x + rect->dados_rect.w, circ->x + circ->dados_circ.r);
    h = fmaxf(rect->y + rect->dados_rect.h, circ->y + circ->dados_circ.r);
  }

  w -= x;
  h -= y;

  x -= STROKE_SIZE;
  y -= STROKE_SIZE;
  w += 2 * STROKE_SIZE;
  h += 2 * STROKE_SIZE;

  return cria_retangulo(x, y, w, h, "", "");
}

float distancia_figuras(Figura f, Figura f2) {
  float distancia;
  float x1, y1, x2, y2;

  get_centro_massa(f, &x1, &y1);

  get_centro_massa(f2, &x2, &y2);

  distancia = dist(x1, y1, x2, y2);

  return distancia;
}

void destruir_figura(Figura f) {
  struct Figura *this = (struct Figura *) f;

  free(this->cor);
  free(this->cor_borda);

  free(f);
}

/** Getters */

float get_x(Figura f) {
  return ((struct Figura *) f)->x;
}

float get_y(Figura f) {
  return ((struct Figura *) f)->y;
}

float get_r(Figura f) {
  struct Figura *this = (struct Figura *) f;

  if (this->tipo != CIRCULO)
    return -1;

  return this->dados_circ.r;
}

float get_w(Figura f) {
  struct Figura *this = (struct Figura *) f;

  if (this->tipo != RETANGULO)
    return -1;

  return this->dados_rect.w;
}

float get_h(Figura f) {
  struct Figura *this = (struct Figura *) f;

  if (this->tipo != RETANGULO)
    return -1;

  return this->dados_rect.h;
}

char *get_cor(Figura f) {
  return ((struct Figura *) f)->cor;
}

char *get_cor_borda(Figura f) {
  return ((struct Figura *) f)->cor_borda;
}

enum TipoFigura get_tipo_figura(Figura f) {
  return ((struct Figura *) f)->tipo;
}