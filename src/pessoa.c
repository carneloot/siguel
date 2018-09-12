#include "pessoa.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct Pessoa {
  char *cpf;
  char *nome;
  char *sobrenome;
  enum Sexo sexo;
  char *nasc;

  Endereco endereco;
  char *complemento;
};

Pessoa cria_pessoa(char *cpf, char *nome, char *sobrenome, int sexo, char *nasc) {
  struct Pessoa *this = malloc(sizeof(*this));

  this->cpf         = malloc(strlen(cpf) + 1);
  this->nome        = malloc(strlen(nome) + 1);
  this->sobrenome   = malloc(strlen(sobrenome) + 1);
  this->nasc        = malloc(strlen(nasc) + 1);
  this->complemento = NULL;
  this->endereco    = NULL;
  strcpy(this->cpf, cpf);
  strcpy(this->nome, nome);
  strcpy(this->sobrenome, sobrenome);
  strcpy(this->nasc, nasc);

  this->sexo = sexo;

  return this;
}

void pessoa_set_endereco(Pessoa _this, char *cep, int face, int num, char *complemento) {
  struct Pessoa *this = (struct Pessoa *) _this;

  if (this->endereco) {
    endereco_destruir(this->endereco);
    free(this->complemento);
  }

  this->complemento = malloc(strlen(complemento) + 1);
  strcpy(this->complemento, complemento);

  this->endereco = cria_endereco(cep, face, num);
}

char *pessoa_get_cpf(Pessoa _this) {
  struct Pessoa * this = (struct Pessoa *) _this;
  return this->cpf;
}

char *pessoa_get_cep(Pessoa _this) {
  struct Pessoa * this = (struct Pessoa *) _this;
  if (this->endereco)
    return this->endereco->cep;
  return NULL;
}

char *pessoa_get_nome(Pessoa _this) {
  struct Pessoa * this = (struct Pessoa *) _this;
  return this->nome;
}

char *pessoa_get_info(Pessoa _this, void *controlador) {
  struct Pessoa * this = (struct Pessoa *) _this;

  char *saida;
  char *endereco_info = endereco_get_info(this->endereco);

  size_t length = 40 + strlen(this->nome) + strlen(this->sobrenome)
    + strlen(this->cpf) + strlen(this->complemento)
    + strlen(endereco_info);

  saida = malloc(length);

  Ponto2D pos;
  if (controlador)
    pos = endereco_get_coordenada(this->endereco, controlador);
  else
    pos = Ponto2D_t.new(0, 0);

  sprintf(saida, "Pessoa %s %s:\n  %s\n  %s (%s)\n  em (%.0f,%.0f)",
    this->nome, this->sobrenome, this->cpf,
    endereco_info, this->complemento,
    pos.x, pos.y
  );

  free(endereco_info);

  return saida;
}

Endereco pessoa_get_endereco(Pessoa _this) {
  struct Pessoa * this = (struct Pessoa *) _this;
  return this->endereco;
}

void pessoa_destruir(Pessoa _this) {
  struct Pessoa *this = (struct Pessoa *) _this;

  free(this->cpf);
  free(this->nome);
  free(this->sobrenome);
  free(this->nasc);
  
  if (this->complemento)
    free(this->complemento);

  if (this->endereco)
    endereco_destruir(this->endereco);

  free(this);
}