#ifndef __ARESTA_H__
#define __ARESTA_H__

struct ArestaInfo {
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
  double velocidade_media
);

void destroy_aresta_info(void *this);

#endif /* __ARESTA_H__ */