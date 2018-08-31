#include "pessoa.h"

#include <stdlib.h>
#include <string.h>

struct Endereco {
  char *cep;
  enum Face face;
  int numero;
  int complemento; // Nao sei se é int ou string
};

struct Pessoa {
  char *cpf;
  char *nome;
  char *sobrenome;
  enum Sexo sexo;
  char *nasc;

  struct Endereco *endereco;
};

static struct Endereco *__criar_endereco(
  char *cep, enum Face face, int numero, int complemento) {
  struct Endereco *this = malloc(sizeof(*this));

  this->cep         = cep;
  this->face        = face;
  this->numero      = numero;
  this->complemento = complemento;

  return this;
}

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
  this->endereco = NULL;

  return this;
}

void pessoa_set_endereco(Pessoa _this, char *cep, int face, int num, int complemento) {
  struct Pessoa *this = (struct Pessoa *) _this;

  this->endereco = __criar_endereco(
    cep, face, num, complemento);
}

void pessoa_destruir(Pessoa _this) {
  struct Pessoa *this = (struct Pessoa *) _this;

  if (this->endereco)
    free(this->endereco);

  free(this->cpf);
  free(this->nome);
  free(this->sobrenome);
  free(this->nasc);

  free(this);
}