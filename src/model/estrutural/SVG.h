#ifndef __SVG_H__
#define __SVG_H__

#include <model/estrutural/desenhavel.h>
#include <model/estrutural/ponto2d.h>

#include <model/trabalho/elemento.h>
#include <model/trabalho/figura.h>

#include <stdbool.h>

#define ASSETS_FOLDER "./assets/" 

typedef void *SVG;

SVG cria_SVG(char *path, double max_width, double max_height);

void desenha_figura(SVG this, Figura f, float opacity, int is_dashed);

void desenha_elemento(SVG this, Elemento e);

void desenha_asset(SVG this, Ponto2D_t pos, Ponto2D_t size, char *nome);

void escreve_texto(
  SVG this, char *texto, Ponto2D_t pos, float tamanho, char *cor);

void escreve_comentario(SVG this, char *texto, ...);

void desenha_linha(SVG this, Ponto2D_t a, Ponto2D_t b, float opacity, double tamanho, char *cor, bool seta);

void desenha_desenhavel(SVG this, Desenhavel desenhavel);

void salva_SVG(SVG this);

void destruir_SVG(SVG this);

#endif /* __SVG_H__ */