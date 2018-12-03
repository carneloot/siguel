#include <comando.r>
#include <controlador.r>

#include <modules/logger.h>
#include <stdlib.h>
#include <string.h>

/**
 * Comando: t
 * Params:  codt descricao
 * Define um tipo de estabelecimento comercial
 */
int comando_ec_t(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;

  char *chave;
  char *valor;

  chave = malloc(strlen(params[0]) + 1);
  strcpy(chave, params[0]);

  valor = malloc(strlen(params[1]) + 1);
  strcpy(valor, params[1]);

  HashTable_t.insert(controlador->tabelas[TIPO_X_DESCRICAO], chave, valor);

  return 1;
}