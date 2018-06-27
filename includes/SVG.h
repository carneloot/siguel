#ifndef __SVG_H__
#define __SVG_H__

#include "figura.h"

enum FiguraTracejada { SVG_BORDA_SOLIDA, SVG_BORDA_TRACEJADA };

typedef void *SVG;

SVG cria_SVG(char *path, float, float);

void desenha_figura(SVG s, Figura f, float opacity, int is_dashed);

void escreve_texto(
  SVG s, char *texto, float x, float y, float tamanho, char *cor);

void desenha_linha(SVG s, float x1, float y1, float x2, float y2, char *cor);

void salva_SVG(SVG s);

void destruir_SVG(SVG s);

#endif /* __SVG_H__ */