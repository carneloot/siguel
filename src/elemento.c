#include "elemento.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct Elemento {
  float x, y;
  char *cor, *cor_borda;

  char *cep;
  enum TipoElemento tipo;

  // ===== ESPECIFICO DE ELEMENTO =====
  union {
    // ===== QUADRA =====
    struct {
      float largura, altura;
    } quadra;

    // ===== HIDRANTE =====
    // struct {
    // } hidrante;

    // ===== SEMAFORO =====
    // struct {
    // } semaforo;

    // ===== RADIO-BASE =====
    // struct {
    // } radio_base;

  } data;
};

static struct Elemento *cria_elemento(float x, float y, char *cep) {
  struct Elemento *this = (struct Elemento *) malloc(sizeof(struct Elemento));

  this->cor       = NULL;
  this->cor_borda = NULL;

  this->cep = calloc(strlen(cep) + 1, sizeof(char));
  this->x   = x;
  this->y   = y;

  strcpy(this->cep, cep);

  return this;
}

// ===== Metodos publicos =====

Elemento cria_quadra(float x, float y, char *cep, float largura, float altura) {
  struct Elemento *this = cria_elemento(x, y, cep);

  this->tipo = QUADRA;

  this->data.quadra.largura = largura;
  this->data.quadra.altura  = altura;

  return (Elemento) this;
}

Elemento cria_hidrante(float x, float y, char *id) {
  struct Elemento *this = cria_elemento(x, y, id);

  this->tipo = HIDRANTE;

  return (Elemento) this;
}

Elemento cria_semaforo(float x, float y, char *id) {
  struct Elemento *this = cria_elemento(x, y, id);

  this->tipo = HIDRANTE;

  return (Elemento) this;
}

Elemento cria_radio_base(float x, float y, char *id) {
  struct Elemento *this = cria_elemento(x, y, id);

  this->tipo = RADIO_BASE;

  return (Elemento) this;
}

void set_cor_elemento(Elemento e, char *cor) {
  struct Elemento *this = (struct Elemento *) e;

  this->cor = calloc(strlen(cor) + 1, sizeof(char));
  strcpy(this->cor, cor);
}

void set_cor_borda_elemento(Elemento e, char *cor_borda) {
  struct Elemento *this = (struct Elemento *) e;

  this->cor_borda = calloc(strlen(cor_borda) + 1, sizeof(char));
  strcpy(this->cor_borda, cor_borda);
}

Figura get_figura_elemento(Elemento e) {
  struct Elemento *this = (struct Elemento *) e;
  Figura figura;

  switch (this->tipo) {
    case QUADRA:
      figura = cria_retangulo(
        this->x,
        this->y,
        this->data.quadra.largura,
        this->data.quadra.altura,
        this->cor,
        this->cor_borda);
      break;
    case HIDRANTE:
    case RADIO_BASE:
    case SEMAFORO:
      figura = cria_circulo(
        this->x, this->y, RAIO_EQUIPAMENTOS, this->cor, this->cor_borda);
      break;
  }

  return figura;
}

void destruir_elemento(Elemento e) {
  struct Elemento *this = (struct Elemento *) e;

  if (this->cor)
    free(this->cor);

  if (this->cor_borda)
    free(this->cor_borda);

  if (this->cep)
    free(this->cep);

  free(this);
}

char *get_info_elemento(Elemento e) {
  struct Elemento *this = (struct Elemento *) e;
  char *saida;
  size_t length;

  switch (this->tipo) {
    case QUADRA:
      length = 28 + strlen(this->cep) + 4 * 8;
      saida = (char *) calloc(length, sizeof(char));
      sprintf(saida,
        "Quadra: %s em (%5.2f,%5.2f) tamanho (%5.2f,%5.2f)",
        this->cep,
        this->x, this->y,
        this->data.quadra.largura, this->data.quadra.altura
      );
      break;

    case HIDRANTE:
      length = 18 + strlen(this->cep) + 2 * 8;
      saida = (char *) calloc(length, sizeof(char));
      sprintf(saida,
        "Hidrante: %s em (%5.2f,%5.2f)",
        this->cep,
        this->x, this->y
      );
      break;

    case RADIO_BASE:
      length = 20 + strlen(this->cep) + 2 * 8;
      saida = (char *) calloc(length, sizeof(char));
      sprintf(saida,
        "Radio Base: %s em (%5.2f,%5.2f)",
        this->cep,
        this->x, this->y
      );
      break;

    case SEMAFORO:
      length = 18 + strlen(this->cep) + 2 * 8;
      saida = (char *) calloc(length, sizeof(char));
      sprintf(saida,
        "Semaforo: %s em (%5.2f,%5.2f)",
        this->cep,
        this->x, this->y
      );
      break;
  }

  return saida;
}

/**
 * Getters
 */

// get_x e get_y são os mesmos da figura;

float get_largura(Elemento e) {
  return ((struct Elemento *) e)->data.quadra.largura;
}

float get_altura(Elemento e) {
  return ((struct Elemento *) e)->data.quadra.altura;
}

char *get_id_elemento(Elemento e) {
  return ((struct Elemento *) e)->cep;
}

char *get_cep_elemento(Elemento e) {
  return ((struct Elemento *) e)->cep;
}

enum TipoElemento get_tipo_elemento(Elemento e) {
  return ((struct Elemento *) e)->tipo;
}
