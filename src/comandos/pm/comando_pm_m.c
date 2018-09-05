#include <comando.r>
#include <controlador.r>

#include <stdlib.h>
#include <pessoa.h>
#include <modules/logger.h>
#include <endereco.h>

int __comando_pm_m(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;
  char *cpf     = params[0];

  HashTable tabela = controlador->tabelas[CPF_X_PESSOA];

  if (!HashTable_t.exists(tabela, cpf)) {
    LOG_ERRO("CPF: \"%s\" não existe na tabela.", cpf);
    return 0;
  }

  HashInfo info = HashTable_t.get(tabela, cpf);
  Pessoa pessoa = info.valor;

  int face   = char_to_face(params[2][0]);
  int numero = strtol(params[3], NULL, 10);

  pessoa_set_endereco(pessoa, params[1], face, numero, params[4]);

  return 1;
}