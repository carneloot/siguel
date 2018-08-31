#include "endereco.h"

Endereco cria_endereco(char *cep, enum Face face, int numero) {
  Endereco this;

  this.cep    = cep;
  this.face   = face;
  this.numero = numero;

  return this;
}