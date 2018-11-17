#include "svg_path.h"

#include <stdlib.h>
#include <string.h>

#include <model/modules/ponto2d.h>
#include <model/utils.h>

void *cria_path(Lista pontos, double tamanho, double opacity, char *cor) {
  struct Path *this = calloc(1, sizeof(*this));

  this->pontos  = pontos;
  this->tamanho = tamanho;
  this->opacity = opacity;
  this->cor     = format_string(cor);

  return this;
}

static char *gerar_string_pontos(Lista pontos) {
  size_t length = Lista_t.length(pontos) * 14;

  char *result = calloc(length + 1, sizeof(char));

  Posic it = Lista_t.get_first(pontos);
  while (it) {
    Ponto2D *ponto = Lista_t.get(pontos, it);

    char *ponto_string = format_string("%6.1f %6.1f,",
      ponto->x, ponto->y);

    strcat(result, ponto_string);

    free(ponto_string);
    
    it = Lista_t.get_next(pontos, it);
  }

  return result;
}

char *print_path(void *_this) {
  struct Path *this = (struct Path *) _this;

  char *pontos_string = gerar_string_pontos(this->pontos);
  
  char *resultado = format_string(
    "<path d=\"M %s\" stroke='%s' "
    "stroke-width='%.1f' fill='transparent' style='opacity: %.1f' />\n",
    pontos_string,
    this->cor, this->tamanho, this->opacity
  );

  free(pontos_string);

  return resultado;
}

void free_path(void *_this) {
  struct Path *this = (struct Path *) _this;
  
  free(this->cor);

  free(this);

}