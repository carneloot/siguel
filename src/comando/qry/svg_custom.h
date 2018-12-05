#ifndef __SVG_CUSTOM_H__
#define __SVG_CUSTOM_H__

#include <model/estrutural/ponto2d.h>

struct Custom {
  Ponto2D_t pos;
  double tamanho;
  char *string;
  char *cor;
};

void *cria_custom(Ponto2D_t pos, double tamanho, char *string, char *cor);

char *print_custom_asset(void *_custom);

char *print_custom_texto(void *_custom);

void free_custom(void *_custom);

#endif /* __SVG_CUSTOM_H__ */