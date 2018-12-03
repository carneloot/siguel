#include <comando.r>
#include <controlador.r>

#include <stdlib.h>

#include <model/trabalho/pessoa.h>
#include <model/trabalho/endereco.h>

#include <utils/logger.h>
#include <utils/utils.h>

/**
 * Comando: m
 * Params:  cpf cep face num compl
 * Informa que um dada pessoa (cpf)
 * mora num dado endereço
 * (cep,face,num,compl)
 */
int comando_pm_m(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;
  char *cpf     = params[0];

  HashTable_t tabela = controlador->tabelas[CPF_X_PESSOA];

  if (!ht_exists(tabela, cpf)) {
    lt_insert(controlador->saida,
      format_string("CPF \"%s\" nao encontrado.\n", cpf));
    return 0;
  }

  Pessoa pessoa = ht_get(tabela, cpf);

  if (pessoa_get_endereco(pessoa) != NULL) {
    ht_remove(controlador->tabelas[CPF_X_CEP], cpf);
  }

  int face   = char_to_face(params[2][0]);
  int numero = strtol(params[3], NULL, 10);
  char *cep  = params[1];
  char *complemento = params[4];

  pessoa_set_endereco(pessoa, cep, face, numero, complemento);

  // Adicionar na tabela CPF_X_CEP
  ht_insert(controlador->tabelas[CPF_X_CEP], pessoa_get_cpf(pessoa), pessoa_get_cep(pessoa));

  return 1;
}