#ifndef __ARESTA_H__
#define __ARESTA_H__

#include <model/modules/ponto2d.h>

struct ArestaInfo {
  Ponto2D pos;

  char *nome;

  char *origem;
  char *destino;

  char *quadra_direita;
  char *quadra_esquerda;

  double comprimento;
  double velocidade_media;
};

typedef struct ArestaInfo *ArestaInfo;

ArestaInfo create_aresta_info(
  char *nome,
  char *origem,
  char *destino,
  char *quadra_direita,
  char *quadra_esquerda,
  double comprimento,
  double velocidade_media,
  Ponto2D pos
);

void destroy_aresta_info(void *this);

void set_aresta_invalido( ArestaInfo this );

void set_aresta_valido( ArestaInfo this, double velocidade_media, double comprimento );

#endif /* __ARESTA_H__ */