#include "endereco.h"

#include <stdlib.h>
#include <string.h>

Endereco cria_endereco(char *cep, enum Face face, int numero) {
  Endereco this = malloc(sizeof(*this));

  this->cep    = malloc(strlen(cep) + 1);
  this->face   = face;
  this->numero = numero;

  strcpy(this->cep, cep);

  return this;
}

void endereco_destruir(Endereco this) {
  free(this->cep);
  free(this);
}

int char_to_face(char indicador) {
  int face;

  switch (indicador) {
    case 'N': face = FACE_NORTE; break;
    case 'S': face = FACE_SUL;   break;
    case 'L': face = FACE_LESTE; break;
    case 'O': face = FACE_OESTE; break;
  }
  
  return face;
}