#include "elemento.h"

#include <modules/ponto2d.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define x(a) ((a)->pos.x)
#define y(a) ((a)->pos.y)

#define largura(a) ((a)->data.quadra.largura)
#define altura(a) ((a)->data.quadra.altura)

struct Elemento {
  Ponto2D pos;
  char *cor, *cor_borda;

  char *cep;
  enum TipoElemento tipo;

  // ===== ESPECIFICO DE ELEMENTO =====
  union {
    // ===== QUADRA =====
    struct {
      double largura, altura;
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

static struct Elemento *cria_elemento(double x, double y, char *cep) {
  struct Elemento *this = (struct Elemento *) malloc(sizeof(struct Elemento));

  this->cor       = NULL;
  this->cor_borda = NULL;

  this->cep = calloc(strlen(cep) + 1, sizeof(char));
  this->pos = Ponto2D_t.new(x, y);

  strcpy(this->cep, cep);

  return this;
}

// ===== Metodos publicos =====

Elemento cria_quadra(
  double x, double y, char *cep, double largura, double altura) {
  struct Elemento *this = cria_elemento(x, y, cep);

  this->tipo = QUADRA;

  largura(this) = largura;
  altura(this)  = altura;

  return (Elemento) this;
}

Elemento cria_hidrante(double x, double y, char *id) {
  struct Elemento *this = cria_elemento(x, y, id);

  this->tipo = HIDRANTE;

  return (Elemento) this;
}

Elemento cria_semaforo(double x, double y, char *id) {
  struct Elemento *this = cria_elemento(x, y, id);

  this->tipo = SEMAFORO;

  return (Elemento) this;
}

Elemento cria_radio_base(double x, double y, char *id) {
  struct Elemento *this = cria_elemento(x, y, id);

  this->tipo = RADIO_BASE;

  return (Elemento) this;
}

void set_cor_elemento(Elemento e, char *cor) {
  struct Elemento *this = (struct Elemento *) e;

  if (this->cor)
    free(this->cor);

  this->cor = calloc(strlen(cor) + 1, sizeof(char));
  strcpy(this->cor, cor);
}

void set_cor_borda_elemento(Elemento e, char *cor_borda) {
  struct Elemento *this = (struct Elemento *) e;

  if (this->cor_borda)
    free(this->cor_borda);

  this->cor_borda = calloc(strlen(cor_borda) + 1, sizeof(char));
  strcpy(this->cor_borda, cor_borda);
}

Figura get_figura_elemento(Elemento e) {
  struct Elemento *this = (struct Elemento *) e;
  Figura figura;

  switch (this->tipo) {
    case QUADRA:
      figura = cria_retangulo(
        x(this),
        y(this),
        largura(this),
        altura(this),
        this->cor,
        this->cor_borda);
      break;
    case HIDRANTE:
    case RADIO_BASE:
    case SEMAFORO:
      figura = cria_circulo(
        x(this), y(this), RAIO_EQUIPAMENTOS, this->cor, this->cor_borda);
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
      saida  = (char *) calloc(length, sizeof(char));
      sprintf(
        saida,
        "Quadra: %s em (%5.2f,%5.2f) tamanho (%5.2f,%5.2f)",
        this->cep,
        x(this),
        y(this),
        largura(this),
        altura(this));
      break;

    case HIDRANTE:
      length = 18 + strlen(this->cep) + 2 * 8;
      saida  = (char *) calloc(length, sizeof(char));
      sprintf(
        saida, "Hidrante: %s em (%5.2f,%5.2f)", this->cep, x(this), y(this));
      break;

    case RADIO_BASE:
      length = 20 + strlen(this->cep) + 2 * 8;
      saida  = (char *) calloc(length, sizeof(char));
      sprintf(
        saida, "Radio Base: %s em (%5.2f,%5.2f)", this->cep, x(this), y(this));
      break;

    case SEMAFORO:
      length = 18 + strlen(this->cep) + 2 * 8;
      saida  = (char *) calloc(length, sizeof(char));
      sprintf(
        saida, "Semaforo: %s em (%5.2f,%5.2f)", this->cep, x(this), y(this));
      break;
  }

  return saida;
}

char *to_string_elemento(Elemento _this) {
  struct Elemento *this = (struct Elemento *) _this;
  char *saida = malloc(strlen(this->cep) + 1);
  strcpy(saida, this->cep);
  return saida;
}

/**
 * Getters
 */

// get_x e get_y são os mesmos da figura;

double get_largura(Elemento e) {
  return largura((struct Elemento *) e);
}

double get_altura(Elemento e) {
  return altura((struct Elemento *) e);
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

const char *get_tipo_string_elemento(enum TipoElemento tipo) {
  const char *elementos[] = {
    [QUADRA]     = "Quadra",
    [HIDRANTE]   = "Hidrante",
    [SEMAFORO]   = "Semaforo",
    [RADIO_BASE] = "Radio Base"
  };

  return elementos[(int) tipo];
}