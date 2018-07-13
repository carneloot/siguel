#include "figura.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

#define STROKE_SIZE 2

#define w(f) ((f)->data.rect.w)
#define h(f) ((f)->data.rect.h)
#define r(f) ((f)->data.circ.r)

struct Figura {
  float x, y;
  char *cor, *cor_borda;

  int id;

  enum TipoFigura tipo;

  /** DADOS DE CADA TIPO */
  union {
    /** DADOS CIRCULO */
    struct {
      float r;
    } circ;

    /** DADOS RETANGULO */
    struct {
      float h, w;
    } rect;
  } data;
};

Figura cria_retangulo(
  float x, float y, float w, float h, char *cor, char *cor_borda) {
  struct Figura *this;
  this = (struct Figura *) malloc(sizeof(struct Figura));

  this->tipo        = RETANGULO;
  this->x           = x;
  this->y           = y;
  this->data.rect.w = w;
  this->data.rect.h = h;

  this->id = -1;

  this->cor       = trim(cor);
  this->cor_borda = trim(cor_borda);

  return (Figura) this;
}

Figura cria_circulo(float x, float y, float r, char *cor, char *cor_borda) {
  struct Figura *this;

  this = (struct Figura *) malloc(sizeof(struct Figura));

  this->tipo        = CIRCULO;
  this->x           = x;
  this->y           = y;
  this->data.circ.r = r;

  this->id = -1;

  this->cor       = trim(cor);
  this->cor_borda = trim(cor_borda);

  return (Figura) this;
}

int sobrepoe_figura(Figura f, Figura f2) {
  struct Figura *this  = (struct Figura *) f;
  struct Figura *other = (struct Figura *) f2;
  struct Figura *circ, *rect;
  float x_perto, y_perto;

  if (this->tipo == CIRCULO && other->tipo == CIRCULO) {
    return (
      dist_squared(this->x, this->y, other->x, other->y) <=
      pow(this->data.circ.r + other->data.circ.r, 2));
  }

  if (this->tipo == RETANGULO && other->tipo == RETANGULO) {
    return !(
      this->x > other->x + other->data.rect.w ||
      this->x + this->data.rect.w < other->x ||
      this->y > other->y + other->data.rect.h ||
      this->y + this->data.rect.h < other->y);
  }

  if (this->tipo == CIRCULO) {
    circ = this;
    rect = other;
  } else {
    circ = other;
    rect = this;
  }

  x_perto = max(rect->x, min(circ->x, rect->x + rect->data.rect.w));
  y_perto = max(rect->y, min(circ->y, rect->y + rect->data.rect.h));

  return (
    dist_squared(circ->x, circ->y, x_perto, y_perto) <=
    pow(circ->data.circ.r, 2));
}

int contem_ponto(Figura f, double x, double y) {
  struct Figura *this;
  this = (struct Figura *) f;

  if (this->tipo == CIRCULO) {
    return (dist_squared(this->x, this->y, x, y) < pow(this->data.circ.r, 2));
  }

  if (this->tipo == RETANGULO) {
    return !(
      x < this->x || x > this->x + this->data.rect.w || y < this->y ||
      y > this->y + this->data.rect.h);
  }

  return 0;
}

void get_centro_massa(Figura f, float *x, float *y) {
  struct Figura *this;
  this = (struct Figura *) f;

  *x = this->x;
  *y = this->y;

  if (this->tipo == RETANGULO) {
    *x += this->data.rect.w / 2;
    *y += this->data.rect.h / 2;
  }
}

Figura get_rect_sobreposicao(Figura f1, Figura f2) {
  struct Figura *this  = (struct Figura *) f1;
  struct Figura *other = (struct Figura *) f2;

  float x, y, h, w;

  if (this->tipo == CIRCULO && other->tipo == CIRCULO) {
    x = min(this->x - this->data.circ.r, other->x - other->data.circ.r);
    y = min(this->y - this->data.circ.r, other->y - other->data.circ.r);

    w = max(this->x + this->data.circ.r, other->x + other->data.circ.r);
    h = max(this->y + this->data.circ.r, other->y + other->data.circ.r);
  }

  else if (this->tipo == RETANGULO && other->tipo == RETANGULO) {
    x = min(this->x, other->x);
    y = min(this->y, other->y);

    w = max(this->x + this->data.rect.w, other->x + other->data.rect.w);
    h = max(this->y + this->data.rect.h, other->y + other->data.rect.h);
  }

  else {
    struct Figura *circ, *rect;

    if (this->tipo == CIRCULO) {
      circ = this;
      rect = other;
    } else {
      circ = other;
      rect = this;
    }

    x = min(rect->x, circ->x - circ->data.circ.r);
    y = min(rect->y, circ->y - circ->data.circ.r);

    w = max(rect->x + rect->data.rect.w, circ->x + circ->data.circ.r);
    h = max(rect->y + rect->data.rect.h, circ->y + circ->data.circ.r);
  }

  w -= x;
  h -= y;

  x -= STROKE_SIZE;
  y -= STROKE_SIZE;
  w += 2 * STROKE_SIZE;
  h += 2 * STROKE_SIZE;

  return cria_retangulo(x, y, w, h, "transparent", "purple");
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
  struct Figura *this;
  this = (struct Figura *) f;

  free(this->cor);
  free(this->cor_borda);

  free(f);
}

int dentro_figura(Figura _this, Figura _other) {
  struct Figura *this  = (struct Figura *) _this;
  struct Figura *other = (struct Figura *) _other;

  if (this->tipo == CIRCULO && other->tipo == RETANGULO) {
    int result;
    float x, y;

    x = other->x;
    y = other->y;

    result = contem_ponto(this, x, y);

    x += w(other);
    result &= contem_ponto(this, x, y);

    x = other->x;
    y += h(other);
    result &= contem_ponto(this, x, y);

    x += w(other);
    result &= contem_ponto(this, x, y);

    return result;
  }

  if (this->tipo == RETANGULO && other->tipo == RETANGULO)
    return (
      other->x >= this->x && other->y >= this->y &&
      other->x + w(other) <= this->x + w(this) &&
      other->y + h(other) <= this->y + h(this));

  return -1;
}

/** Getters */

float get_x(Figura f) {
  return ((struct Figura *) f)->x;
}

float get_y(Figura f) {
  return ((struct Figura *) f)->y;
}

float get_r(Figura f) {
  struct Figura *this;
  this = (struct Figura *) f;

  if (this->tipo != CIRCULO)
    return 0;

  return this->data.circ.r;
}

float get_w(Figura f) {
  struct Figura *this;
  this = (struct Figura *) f;

  if (this->tipo != RETANGULO)
    return 0;

  return this->data.rect.w;
}

float get_h(Figura f) {
  struct Figura *this;
  this = (struct Figura *) f;

  if (this->tipo != RETANGULO)
    return 0;

  return this->data.rect.h;
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

int get_id_figura(Figura f) {
  return ((struct Figura *) f)->id;
}

void set_id_figura(Figura f, int id) {
  ((struct Figura *) f)->id = id;
}