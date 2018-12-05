#ifndef __ENDERECO_H__
#define __ENDERECO_H__

#include <model/estrutural/ponto2d.h>

enum Face { FACE_NORTE, FACE_LESTE, FACE_SUL, FACE_OESTE };

struct Endereco {
  char *cep;
  enum Face face;
  int numero;
};

typedef struct Endereco *Endereco;

Endereco cria_endereco(char *cep, enum Face face, int numero);

void endereco_destruir(Endereco this);

Ponto2D_t endereco_get_coordenada(Endereco this, void *controlador);

char *endereco_get_info(Endereco this);

int char_to_face(char indicador);

char face_to_char(int indicador);

#endif /* __ENDERECO_H__ */