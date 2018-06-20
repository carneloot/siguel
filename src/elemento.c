#include "elemento.h"

#include <stdlib.h>
#include <string.h>

#define RAIO_EQUIPAMENTOS 5

struct Elemento {
  enum TipoElemento tipo;

  float x, y;
  int id;

  char *cor;
  char *cor_borda;

  // ===== ESPECIFICO DE ELEMENTO =====
  union {
    // ===== QUADRA =====
    struct {
      char *cep;
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

static struct Elemento *cria_elemento(int id, float x, float y) {
  struct Elemento *this = (struct Elemento *) malloc(sizeof(struct Elemento));

  this->cor       = NULL;
  this->cor_borda = NULL;

  this->id = id;
  this->x  = x;
  this->y  = y;

  return this;
}

// ===== Metodos publicos =====

Elemento cria_quadra(float x, float y, char *cep, float largura, float altura) {
  struct Elemento *this = cria_elemento(0, x, y);

  this->tipo = QUADRA;

  this->data.quadra.cep = (char *) calloc(sizeof(cep) + 1, sizeof(char));
  strcpy(cep, this->data.quadra.cep);

  this->data.quadra.largura = largura;
  this->data.quadra.altura  = altura;

  return (Elemento) this;
}

Elemento cria_hidrante(int id, float x, float y) {
  struct Elemento *this = cria_elemento(id, x, y);

  this->tipo = HIDRANTE;

  return (Elemento) this;
}

Elemento cria_semaforo(int id, float x, float y) {
  struct Elemento *this = cria_elemento(id, x, y);

  this->tipo = HIDRANTE;

  return (Elemento) this;
}

Elemento cria_radio_base(int id, float x, float y) {
  struct Elemento *this = cria_elemento(id, x, y);

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

  // QUADRA
  if (this->tipo == QUADRA)
    free(this->data.quadra.cep);

  free(this);
}
