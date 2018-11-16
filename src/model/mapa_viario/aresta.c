#include "aresta.h"

#include <stdlib.h>
#include <string.h>

ArestaInfo create_aresta_info(
  char *nome,
  char *origem,
  char *destino,
  char *quadra_direita,
  char *quadra_esquerda,
  double comprimento,
  double velocidade_media) {

  ArestaInfo this = calloc(1, sizeof(*this));

  this->comprimento      = comprimento;
  this->velocidade_media = velocidade_media;

  this->nome            = (char *) calloc(strlen(nome) + 1, sizeof(char));
  this->origem          = (char *) calloc(strlen(origem) + 1, sizeof(char));
  this->destino         = (char *) calloc(strlen(destino) + 1, sizeof(char));
  this->quadra_direita  = (char *) calloc(strlen(quadra_direita) + 1, sizeof(char));
  this->quadra_esquerda = (char *) calloc(strlen(quadra_esquerda) + 1, sizeof(char));

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
