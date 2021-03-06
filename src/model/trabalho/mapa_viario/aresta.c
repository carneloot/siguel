#include "aresta.h"

#include <stdlib.h>
#include <string.h>
#include <float.h>

ArestaInfo create_aresta_info(
  char *nome,
  char *origem,
  char *destino,
  char *quadra_direita,
  char *quadra_esquerda,
  double comprimento,
  double velocidade_media,
  Ponto2D_t pos) {

  ArestaInfo this = calloc(1, sizeof(*this));

  this->comprimento      = comprimento;
  this->velocidade_media = velocidade_media;
  this->comprimento_backup      = comprimento;
  this->velocidade_media_backup = velocidade_media;

  this->nome            = (char *) calloc(strlen(nome) + 1, sizeof(char));
  this->origem          = (char *) calloc(strlen(origem) + 1, sizeof(char));
  this->destino         = (char *) calloc(strlen(destino) + 1, sizeof(char));
  this->quadra_direita  = (char *) calloc(strlen(quadra_direita) + 1, sizeof(char));
  this->quadra_esquerda = (char *) calloc(strlen(quadra_esquerda) + 1, sizeof(char));

  this->pos = pos;

  strcpy(this->nome, nome);
  strcpy(this->origem, origem);
  strcpy(this->destino, destino);
  strcpy(this->quadra_direita, quadra_direita);
  strcpy(this->quadra_esquerda, quadra_esquerda);

  return this;
}

void destroy_aresta_info(void *_this) {
  ArestaInfo this = _this;

  free(this->nome);

  free(this->origem);
  free(this->destino);

  free(this->quadra_direita);
  free(this->quadra_esquerda);

  free(this);

}

void set_aresta_invalido( ArestaInfo this ){
  this->comprimento      = DBL_MAX;
  this->velocidade_media = 0;
}

void set_aresta_valido( ArestaInfo this ){
  this->comprimento      = this->comprimento_backup;
  this->velocidade_media = this->velocidade_media_backup;
}

bool get_aresta_valido(ArestaInfo this) {
  return !(this->comprimento == DBL_MAX && this->velocidade_media == 0);
}