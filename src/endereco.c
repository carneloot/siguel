#include "endereco.h"

Endereco cria_endereco(char *cep, enum Face face, int numero) {
  Endereco this;

  this.cep    = cep;
  this.face   = face;
  this.numero = numero;

  return this;
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