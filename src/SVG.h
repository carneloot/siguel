#ifndef __SVG_H__
#define __SVG_H__

#include "figura.h"

typedef void *SVG;

SVG cria_SVG();

void desenha_figura(SVG s, Figura f);

void escreve_texto(SVG s, char *texto, float x, float y, float tamanho,
                   char *cor, char *cor_borda);

void salva_SVG(char *path);

#endif  // __SVG_H__