#include "ponto2d.h"

#include "math.h"
#include "utils.h"

static Ponto2D __new_ponto2d(double x, double y) {
  Ponto2D this = {.x = x, .y = y};
  return this;
}

static double __dist_squared_ponto2d(Ponto2D this, Ponto2D other) {
  return (sqr(this.x - other.x) + sqr(this.y - other.y));
}

static double __dist_ponto2d(Ponto2D this, Ponto2D other) {
  return sqrt(__dist_squared_ponto2d(this, other));
}

static Ponto2D __mult_ponto2d(Ponto2D this, double scalar) {
  return __new_ponto2d(this.x * scalar, this.y * scalar);
}

static Ponto2D __add_ponto2d(Ponto2D this, Ponto2D other) {
  return __new_ponto2d(this.x + other.x, this.y + other.y);
}

static Ponto2D __sub_ponto2d(Ponto2D this, Ponto2D other) {
  return __new_ponto2d(this.x - other.x, this.y - other.y);
}

static Ponto2D __add_scalar_ponto2d(Ponto2D this, double scalar) {
  return __new_ponto2d(this.x + scalar, this.y + scalar);
}

static Ponto2D __sub_scalar_ponto2d(Ponto2D this, double scalar) {
  return __new_ponto2d(this.x - scalar, this.y - scalar);
}

const struct Ponto2D_t Ponto2D_t = {
  .new          = &__new_ponto2d,
  .dist         = &__dist_ponto2d,
  .dist_squared = &__dist_squared_ponto2d,
  .mult         = &__mult_ponto2d,
  .add          = &__add_ponto2d,
  .sub          = &__sub_ponto2d,
  .add_scalar   = &__add_scalar_ponto2d,
  .sub_scalar   = &__sub_scalar_ponto2d,
};