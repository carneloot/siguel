#ifndef __ELEMENTO_H__
#define __ELEMENTO_H__

#include "figura.h"

#define RAIO_EQUIPAMENTOS 5

typedef void *Elemento;

enum TipoElemento { QUADRA, HIDRANTE, RADIO_BASE, SEMAFORO };

Elemento cria_quadra(
  double x, double y, char *cep, double largura, double altura);

Elemento cria_hidrante(double x, double y, char *id);

Elemento cria_semaforo(double x, double y, char *id);

Elemento cria_radio_base(double x, double y, char *id);

void set_cor_elemento(Elemento e, char *cor);

void set_cor_borda_elemento(Elemento e, char *cor_borda);

void destruir_elemento(Elemento e);

Figura get_figura_elemento(Elemento e);

char *get_info_elemento(Elemento e);

/**
 * Getters
 */

// get_pos, get_x e get_y são os mesmos da figura;

double get_largura(Elemento e);

double get_altura(Elemento e);

char *get_id_elemento(Elemento e);

char *get_cep_elemento(Elemento e);

enum TipoElemento get_tipo_elemento(Elemento e);

#endif /* __ELEMENTO_H__ */