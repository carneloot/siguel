#include "desenhavel.h"

#include <stdlib.h>

Desenhavel cria_desenhavel(
  void *valor, char *(*to_svg)(void *valor), void (*destruir)(void *valor)) {
  struct Desenhavel *this = calloc(1, sizeof(*this));

  this->valor     = valor;
  this->to_svg    = to_svg;
  this->destruir  = destruir;

  return this;
}

void desenhavel_destruir(void *_this) {
  Desenhavel this = (Desenhavel) _this;
  if (this->destruir)
    this->destruir(this->valor);
  free(this);
}