#include "comercio.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <utils.h>

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

void comercio_set_endereco(Comercio _this, char *cep, int face, int num) {
  struct Comercio * this = (struct Comercio *) _this;

  endereco_destruir(this->endereco);
  this->endereco = cria_endereco(cep, face, num);
}

void comercio_destruir(Comercio _this) {
  struct Comercio * this = (struct Comercio *) _this;

  free(this->tipo);
  free(this->cnpj);
  free(this->nome);
  endereco_destruir(this->endereco);

  free(this);
}

char *comercio_get_cnpj(Comercio _this) {
  struct Comercio *this = (struct Comercio *) _this;
  return this->cnpj;  
}

char *comercio_get_tipo(Comercio _this) {
  struct Comercio *this = (struct Comercio *) _this;
  return this->tipo;  
}

char *comercio_get_info(Comercio _this, char *tipo_desc) {
  struct Comercio * this = (struct Comercio *) _this;

  char *endereco_info = endereco_get_info(this->endereco);

  char *saida = format_string(
    "Comercio %s:\n  %s\n  %s\n  %s",
    this->nome, this->cnpj, tipo_desc, endereco_info
  );

  free(endereco_info);

  return saida;
}

Endereco comercio_get_endereco(Comercio _this) {
  struct Comercio *this = (struct Comercio *) _this;
  return this->endereco;
}