#ifndef _SVG_H_
#define _SVG_H_

#include "figura.h"

typedef struct SVG* SVG;

SVG cria_SVG();

void desenha_figura(SVG s, Figura f);

void escreve_texto(SVG s, char* texto, float x, float y, float tamanho, char* cor, char* cor_borda);

void salva_SVG(char* path);

#endif // _SVG_H_