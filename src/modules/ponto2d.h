#ifndef __PONTO2D_H__
#define __PONTO2D_H__

struct Ponto2D {
  double x, y;
};

typedef struct Ponto2D Ponto2D;

struct Ponto2D_t {
  Ponto2D (*new)(double x, double y);

  int (*equal)(Ponto2D this, Ponto2D other);

  double (*dist_squared)(Ponto2D this, Ponto2D other);

  double (*dist)(Ponto2D this, Ponto2D other);

  Ponto2D (*mult)(Ponto2D this, double scalar);

  Ponto2D (*add)(Ponto2D this, Ponto2D other);

  Ponto2D (*sub)(Ponto2D this, Ponto2D other);

  Ponto2D (*add_scalar)(Ponto2D this, double scalar);

  Ponto2D (*sub_scalar)(Ponto2D this, double scalar);

  Ponto2D (*maximo)(Ponto2D this, Ponto2D other);
};

extern const struct Ponto2D_t Ponto2D_t;

#endif /* __PONTO2D_H__ */