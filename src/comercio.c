#include "comercio.h"

#include <stdlib.h>
#include <string.h>

struct Comercio {
  char *tipo;
  char *descricao;
};

Comercio cria_comercio(char *tipo, char *descricao) {
  struct Comercio *this = malloc(sizeof(*this));

  this->tipo      = malloc(strlen(tipo) + 1);
  this->descricao = malloc(strlen(descricao) + 1);

  strcpy(this->tipo, tipo);
  strcpy(this->descricao, descricao);

  return this;
}

void comercio_destruir(Comercio _this) {
  struct Comercio * this = (struct Comercio *) _this;

  free(this->tipo);
  free(this->descricao);

  free(this);
}