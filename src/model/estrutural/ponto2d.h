#ifndef __PONTO2D_H__
#define __PONTO2D_H__

#include <stdbool.h>

struct Ponto2D_t {
  double x, y;
};

typedef struct Ponto2D_t Ponto2D_t;

/**
 * Cria um novo Ponto2D_t com os valores (x, y)
 */
Ponto2D_t p2d_new(double x, double y);

/**
 * Retorna true se os dois pontos passados forem iguais
 */
bool p2d_equal(Ponto2D_t this, Ponto2D_t other);

/**
 * Retorna a distancia ao quadrado entre os dois pontos
 * Boa para usar em iteracoes por nao conter sqrt
 */
double p2d_dist_squared(Ponto2D_t this, Ponto2D_t other);

/**
 * Retorna a distancia entre os dois pontos passados
 */
double p2d_dist(Ponto2D_t this, Ponto2D_t other);

/**
 * Retorna o ponto this multiplicado pelo escalar k
 * Equivalente a k(x, y)
 */
Ponto2D_t p2d_mult(Ponto2D_t this, double k);

/**
 * Retorna a soma vetorial entre this e other
 * Equivalente a (x1, y1) + (x2, y2) = (x1 + x2, y1 + y2)
 */
Ponto2D_t p2d_add(Ponto2D_t this, Ponto2D_t other);

/**
 * Retorna a subtracao vetorial entre this e other
 * Equivalente a (x1, y1) - (x2, y2) = (x1 - x2, y1 - y2)
 */
Ponto2D_t p2d_sub(Ponto2D_t this, Ponto2D_t other);

/**
 * Retona o ponto this com k somado em seus dois componentes
 * Equivalente a (x + k, y + k)
 */
Ponto2D_t p2d_add_scalar(Ponto2D_t this, double k);

/**
 * Retona o ponto this com k subtraido em seus dois componentes
 * Equivalente a (x - k, y - k)
 */
Ponto2D_t p2d_sub_scalar(Ponto2D_t this, double k);

/**
 * Retona o maior ponto entre this e other
 * Equivalente a max((x1, y1), (x2, y2)) = (max(x1, x2), max(y1, y2))
 */
Ponto2D_t p2d_maximo(Ponto2D_t this, Ponto2D_t other);

/**
 * Produto vetorial entre this e other
 * Equivalente a vetorial((x1, y1), (x2, y2)) = x1 * y2 - x2 * y1
 */
double p2d_vetorial(Ponto2D_t this, Ponto2D_t other);

/**
 * Retona o angulo do ponto this a partir da origem
 */
double p2d_angle(Ponto2D_t this);

/**
 * Retorna a orientacao entre tres pontos:
 *  -1: anti-horario (vira a esquerda)
 *   0: colineares
 *  +1:horario (vira a direita)
 */
int p2d_orientation(Ponto2D_t a, Ponto2D_t b, Ponto2D_t c);

#endif /* __PONTO2D_H__ */