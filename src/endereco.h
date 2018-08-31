#ifndef __ENDERECO_H__
#define __ENDERECO_H__

enum Face { FACE_NORTE, FACE_LESTE, FACE_SUL, FACE_OESTE };

struct Endereco {
  char *cep;
  enum Face face;
  int numero;
};

typedef struct Endereco Endereco;

Endereco cria_endereco(char *cep, enum Face face, int numero);

#endif /* __ENDERECO_H__ */