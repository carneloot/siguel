#include "endereco.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <controlador.h>

#include <model/colecao/hash.h>

#include <model/trabalho/elemento.h>

#include <utils/utils.h>

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

Ponto2D_t endereco_get_coordenada(Endereco this, void *controlador) {

  HashTable_t tabela_quadras = get_table_quadras(controlador);

  if (!ht_exists(tabela_quadras, this->cep)) {
    return p2d_new(0,0);
  }

  Elemento quadra = ht_get(tabela_quadras, this->cep);

  Ponto2D_t pos = get_pos(quadra);

  switch (this->face) {
    case FACE_LESTE:
    case FACE_OESTE:
      pos.y += (double) this->numero;
      break;

    case FACE_NORTE:
    case FACE_SUL:
      pos.x += (double) this->numero;
      break;
  }

  if (this->face == FACE_OESTE)
    pos.x += get_largura(quadra);

  if (this->face == FACE_NORTE)
    pos.y += get_altura(quadra);

  return pos;
}

char *endereco_get_info(Endereco this) {
  // "%s, %c - $d"
  char *saida = format_string("%s, %c - %d",
    this->cep, face_to_char(this->face), this->numero
  );

  return saida;
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

char face_to_char(int indicador) {
  char saida;

  switch (indicador) {
    case FACE_NORTE: saida = 'N'; break;
    case FACE_SUL:   saida = 'S'; break;
    case FACE_LESTE: saida = 'L'; break;
    case FACE_OESTE: saida = 'O'; break;
  }
  
  return saida;
}