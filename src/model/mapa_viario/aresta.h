#ifndef __ARESTA_H__
#define __ARESTA_H__

struct ArestaInfo {
  char *nome;
  char *quadra_direita;
  char *quadra_esquerda;
  double comprimento;
  double velocidade_media;
};

typedef struct ArestaInfo *ArestaInfo;

ArestaInfo create_aresta_info(
  char *nome,
  char *quadra_direita,
  char *quadra_esquerda,
  double comprimento,
  double velocidade_media
);

void destroy_aresta_info(ArestaInfo this);

#endif /* __ARESTA_H__ */