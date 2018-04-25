#ifndef __SVG_H__
#define __SVG_H__

#include "figura.h"

typedef void *SVG;

SVG cria_SVG(char *nome_saida);

void desenha_figura(SVG s, Figura f);

void escreve_texto(SVG s, char *texto, float x, float y, float tamanho,
                   char *cor, char *cor_borda);

void desenha_linha(SVG s, float x1, float y1, float x2, float y2);

void salva_SVG(char *path);

void destruir_SVG(SVG s);

#endif  // __SVG_H__