#include "comercio.h"

#include <stdlib.h>
#include <string.h>
#include <endereco.h>

struct Comercio {
  char *cnpj;
  char *tipo;
  char *nome;

  Endereco endereco;
};

Comercio cria_comercio(char *cnpj, char *tipo, char *cep, int face, int num, char *nome) {
  struct Comercio *this = malloc(sizeof(*this));

  this->tipo = malloc(strlen(tipo) + 1);
  this->cnpj = malloc(strlen(cnpj) + 1);
  this->nome = malloc(strlen(nome) + 1);
  strcpy(this->cnpj, cnpj);
  strcpy(this->tipo, tipo);
  strcpy(this->nome, nome);

  this->endereco = cria_endereco(cep, face, num);

  return this;
}

void comercio_destruir(Comercio _this) {
  struct Comercio * this = (struct Comercio *) _this;

  free(this->tipo);
  free(this->cnpj);
  free(this->nome);
  endereco_destruir(this->endereco);

  free(this);
}