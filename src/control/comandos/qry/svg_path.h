#ifndef __SVG_PATH_H__
#define __SVG_PATH_H__

#include <model/modules/lista.h>

struct Path {
  Lista pontos;
  double tamanho;
  double opacity;
  char *cor;
};

void *cria_path(Lista pontos, double tamanho, double opacity, char *cor);

char *print_path(void *this);

void free_path(void *this);

#endif /* __SVG_PATH_H__ */