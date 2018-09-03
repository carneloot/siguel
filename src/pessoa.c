#include "pessoa.h"

#include <stdlib.h>
#include <string.h>

#include <endereco.h>

struct Pessoa {
  char *cpf;
  char *nome;
  char *sobrenome;
  enum Sexo sexo;
  char *nasc;
  char *complemento;

  struct Endereco endereco;
};

Pessoa cria_pessoa(char *cpf, char *nome, char *sobrenome, int sexo, char *nasc) {
  struct Pessoa *this = malloc(sizeof(*this));

  this->cpf       = malloc(strlen(cpf) + 1);
  this->nome      = malloc(strlen(nome) + 1);
  this->sobrenome = malloc(strlen(sobrenome) + 1);
  this->nasc      = malloc(strlen(nasc) + 1);
  strcpy(this->cpf, cpf);
  strcpy(this->nome, nome);
  strcpy(this->sobrenome, sobrenome);
  strcpy(this->nasc, nasc);

  this->sexo     = sexo;

  return this;
}

void pessoa_set_endereco(Pessoa _this, char *cep, int face, int num, char *complemento) {
  struct Pessoa *this = (struct Pessoa *) _this;

  this->complemento = malloc(strlen(complemento) + 1);
  strcpy(this->complemento, complemento);

  this->endereco = cria_endereco(cep, face, num);
}

void pessoa_destruir(Pessoa _this) {
  struct Pessoa *this = (struct Pessoa *) _this;

  free(this->cpf);
  free(this->nome);
  free(this->sobrenome);
  free(this->nasc);

  free(this);
}