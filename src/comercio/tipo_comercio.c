#include "tipo_comercio.h"

struct TipoComercio {
  char *identificacao;
  char *descricao;
};

TipoComercio cria_tipo_comercio(char *identificacao, char *descricao) {
  struct TipoComercio *this = malloc(sizeof(*this));

  this->identificacao = malloc(strlen(identificacao) + 1);
  this->descricao     = malloc(strlen(descricao) + 1);

  strcpy(this->identificacao, identificacao);
  strcpy(this->descricao, descricao);

  return this;
}

void tipo_comercio_destruir(TipoComercio _this) {
  struct TipoComercio * this = (struct TipoComercio *) _this;

  free(this->identificacao);
  free(this->descricao);

  free(this);
}