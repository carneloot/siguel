#include "ponto2d.h"

#include <math.h>

#include <utils/utils.h>

Ponto2D_t p2d_new(double x, double y) {
  Ponto2D_t this = {.x = x, .y = y};
  return this;
}

bool p2d_equal(Ponto2D_t this, Ponto2D_t other) {
  return (this.x == other.x && this.y == other.y);
}

double p2d_dist_squared(Ponto2D_t this, Ponto2D_t other) {
  return (sqr(this.x - other.x) + sqr(this.y - other.y));
}

double p2d_dist(Ponto2D_t this, Ponto2D_t other) {
  return sqrt(p2d_dist_squared(this, other));
}

Ponto2D_t p2d_mult(Ponto2D_t this, double scalar) {
  return p2d_new(this.x * scalar, this.y * scalar);
}

Ponto2D_t p2d_add(Ponto2D_t this, Ponto2D_t other) {
  return p2d_new(this.x + other.x, this.y + other.y);
}

Ponto2D_t p2d_sub(Ponto2D_t this, Ponto2D_t other) {
  return p2d_new(this.x - other.x, this.y - other.y);
}

Ponto2D_t p2d_add_scalar(Ponto2D_t this, double scalar) {
  return p2d_new(this.x + scalar, this.y + scalar);
}

Ponto2D_t p2d_sub_scalar(Ponto2D_t this, double scalar) {
  return p2d_new(this.x - scalar, this.y - scalar);
}

Ponto2D_t p2d_maximo(Ponto2D_t this, Ponto2D_t other) {
  return p2d_new(
    (this.x > other.x) ? this.x : other.x,
    (this.y > other.y) ? this.y : other.y
  );
}

double p2d_vetorial(Ponto2D_t this, Ponto2D_t other) {
  return this.x * other.y - this.y * other.x;
}

double p2d_angle(Ponto2D_t this) {
  return atan2(this.y, this.x);
}

int p2d_orientation(Ponto2D_t a, Ponto2D_t b, Ponto2D_t c) {
  double sigma = (b.y - a.y) / (b.x - a.x);
  double tau   = (c.y - b.y) / (c.x - b.x);

  double result = sigma - tau;

  return (result == 0) ? 0 : (result > 0) ? 1 : -1;
}
