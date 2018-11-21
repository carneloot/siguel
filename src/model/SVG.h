#ifndef __SVG_H__
#define __SVG_H__

#include <model/elemento.h>
#include <model/figura.h>
#include <model/desenhavel.h>
#include <model/modules/ponto2d.h>

#include <stdbool.h>

#define ASSETS_FOLDER "./assets/" 

typedef void *SVG;

SVG cria_SVG(char *path, double max_width, double max_height);

void desenha_figura(SVG this, Figura f, float opacity, int is_dashed);

void desenha_elemento(SVG this, Elemento e);

void desenha_asset(SVG this, Ponto2D pos, Ponto2D size, char *nome);

void escreve_texto(
  SVG this, char *texto, Ponto2D pos, float tamanho, char *cor);

void escreve_comentario(SVG this, char *texto, ...);

void desenha_linha(SVG this, Ponto2D a, Ponto2D b, float opacity, double tamanho, char *cor, bool seta);

void desenha_desenhavel(SVG this, Desenhavel desenhavel);

void salva_SVG(SVG this);

void destruir_SVG(SVG this);

#endif /* __SVG_H__ */