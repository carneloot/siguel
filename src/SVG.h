#ifndef __SVG_H__
#define __SVG_H__

#include <modules/ponto2d.h>
#include "elemento.h"
#include "figura.h"

enum FiguraTracejada { SVG_BORDA_SOLIDA, SVG_BORDA_TRACEJADA };

typedef void *SVG;

SVG cria_SVG(char *path, double max_width, double max_height);

void desenha_figura(SVG s, Figura f, float opacity, int is_dashed);

void desenha_elemento(SVG this, Elemento e);

void escreve_texto(SVG s, char *texto, Ponto2D pos, float tamanho, char *cor);

void desenha_linha(SVG s, Ponto2D a, Ponto2D b, char *cor);

void salva_SVG(SVG s);

void destruir_SVG(SVG s);

#endif /* __SVG_H__ */