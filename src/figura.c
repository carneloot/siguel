#include "figura.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

#define STROKE_SIZE 2

#define x(a) ((a)->pos.x)
#define y(a) ((a)->pos.y)

#define w(f) ((f)->data.rect.w)
#define h(f) ((f)->data.rect.h)
#define r(f) ((f)->data.circ.r)

struct Figura {
  Ponto2D pos;
  char *cor, *cor_borda;

  int id;

  enum TipoFigura tipo;

  /** DADOS DE CADA TIPO */
  union {
    /** DADOS CIRCULO */
    struct {
      double r;
    } circ;

    /** DADOS RETANGULO */
    struct {
      double h, w;
    } rect;
  } data;
};

static struct Figura *__cria_figura(
  double x, double y, char *cor, char *cor_borda) {
  struct Figura *this = (struct Figura *) malloc(sizeof(struct Figura));

  this->pos = Ponto2D_t.new(x, y);
  this->id  = -1;

  this->cor       = trim(cor);
  this->cor_borda = trim(cor_borda);

  return this;
}

Figura cria_retangulo(
  double x, double y, double w, double h, char *cor, char *cor_borda) {
  struct Figura *this = __cria_figura(x, y, cor, cor_borda);

  this->tipo = RETANGULO;
  w(this)    = w;
  h(this)    = h;

  return (Figura) this;
}

Figura cria_circulo(double x, double y, double r, char *cor, char *cor_borda) {
  struct Figura *this = __cria_figura(x, y, cor, cor_borda);

  this->tipo = CIRCULO;
  r(this)    = r;

  return (Figura) this;
}

int sobrepoe_figura(Figura f, Figura f2) {
  struct Figura *this  = (struct Figura *) f;
  struct Figura *other = (struct Figura *) f2;
  struct Figura *circ, *rect;

  if (this->tipo == CIRCULO && other->tipo == CIRCULO) {
    return (
      Ponto2D_t.dist_squared(this->pos, other->pos) <= sqr(r(this) + r(other)));
  }

  if (this->tipo == RETANGULO && other->tipo == RETANGULO) {
    return !(
      x(this) > x(other) + w(other) || x(this) + w(this) < x(other) ||
      y(this) > y(other) + h(other) || y(this) + h(this) < y(other));
  }

  if (this->tipo == CIRCULO) {
    circ = this;
    rect = other;
  } else {
    circ = other;
    rect = this;
  }

  Ponto2D perto = Ponto2D_t.new(
    max(x(rect), min(x(circ), x(rect) + w(rect))),
    max(y(rect), min(y(circ), y(rect) + h(rect))));

  return (Ponto2D_t.dist_squared(circ->pos, perto) <= sqr(r(circ)));
}

int contem_ponto(Figura f, Ponto2D p) {
  struct Figura *this;
  this = (struct Figura *) f;

  if (this->tipo == CIRCULO) {
    return (Ponto2D_t.dist_squared(this->pos, p) < sqr(r(this)));
  }

  if (this->tipo == RETANGULO) {
    return !(
      p.x < x(this) || p.x > x(this) + w(this) ||  //
      p.y < y(this) || p.y > y(this) + h(this));
  }

  return 0;
}

Ponto2D get_centro_massa(Figura f) {
  struct Figura *this;
  this = (struct Figura *) f;

  Ponto2D result = Ponto2D_t.new(x(this), y(this));

  if (this->tipo == RETANGULO) {
    result.x += w(this) / 2;
    result.y += h(this) / 2;
  }

  return result;
}

Figura get_rect_sobreposicao(Figura f1, Figura f2) {
  struct Figura *this  = (struct Figura *) f1;
  struct Figura *other = (struct Figura *) f2;

  Ponto2D pos, size;

  if (this->tipo == CIRCULO && other->tipo == CIRCULO) {
    pos = Ponto2D_t.new(
      min(x(this) - r(this), x(other) - r(other)),
      min(y(this) - r(this), y(other) - r(other)));

    size = Ponto2D_t.new(
      max(x(this) + r(this), x(other) + r(other)),
      max(y(this) + r(this), y(other) + r(other)));
  }

  else if (this->tipo == RETANGULO && other->tipo == RETANGULO) {
    pos = Ponto2D_t.new(  //
      min(x(this), x(other)),
      min(y(this), y(other)));

    size = Ponto2D_t.new(
      max(x(this) + w(this), x(other) + w(other)),
      max(y(this) + h(this), y(other) + h(other)));
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
    pos = Ponto2D_t.new(  //
      min(x(rect), x(circ) - r(circ)),
      min(y(rect), y(circ) - r(circ)));

    size = Ponto2D_t.new(
      max(x(rect) + w(rect), x(circ) + r(circ)),
      max(y(rect) + h(rect), y(circ) + r(circ)));
  }

  size = Ponto2D_t.sub(size, pos);

  pos  = Ponto2D_t.sub_scalar(pos, STROKE_SIZE);
  size = Ponto2D_t.add_scalar(size, 2 * STROKE_SIZE);

  return cria_retangulo(pos.x, pos.y, size.x, size.y, "transparent", "purple");
}

double distancia_figuras(Figura this, Figura other) {
  return Ponto2D_t.dist(get_centro_massa(this), get_centro_massa(other));
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
    Ponto2D pos_other = other->pos;

    result = contem_ponto(this, pos_other);

    pos_other.x += w(other);
    result &= contem_ponto(this, pos_other);

    pos_other.x = x(other);
    pos_other.y += h(other);
    result &= contem_ponto(this, pos_other);

    pos_other.x += w(other);
    result &= contem_ponto(this, pos_other);

    return result;
  }

  if (this->tipo == RETANGULO && other->tipo == RETANGULO)
    return (
      x(other) >= x(this) && y(other) >= y(this) &&
      x(other) + w(other) <= x(this) + w(this) &&
      y(other) + h(other) <= y(this) + h(this));

  return -1;
}

/** Getters */

Ponto2D get_pos(Figura f) {
  return ((struct Figura *) f)->pos;
}

double get_x(Figura f) {
  return x((struct Figura *) f);
}

double get_y(Figura f) {
  return y((struct Figura *) f);
}

double get_r(Figura f) {
  struct Figura *this;
  this = (struct Figura *) f;

  if (this->tipo != CIRCULO)
    return 0;

  return r(this);
}

double get_w(Figura f) {
  struct Figura *this;
  this = (struct Figura *) f;

  if (this->tipo != RETANGULO)
    return 0;

  return w(this);
}

double get_h(Figura f) {
  struct Figura *this;
  this = (struct Figura *) f;

  if (this->tipo != RETANGULO)
    return 0;

  return h(this);
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