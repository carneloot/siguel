#ifndef __ELEMENTO_H__
#define __ELEMENTO_H__

#include "figura.h"

typedef void *Elemento;

enum TipoElemento { QUADRA, HIDRANTE, RADIO_BASE, SEMAFORO };

Elemento cria_quadra(float x, float y, char *cep, float largura, float altura);

Elemento cria_hidrante(int id, float x, float y);

Elemento cria_semaforo(int id, float x, float y);

Elemento cria_radio_base(int id, float x, float y);

void set_cor_elemento(Elemento e, char *cor);

void set_cor_borda_elemento(Elemento e, char *cor_borda);

void destruir_elemento(Elemento e);

Figura get_figura_elemento(Elemento e);

#endif /* __ELEMENTO_H__ */