#include <model/comando.r>
#include <control/controlador.r>

#include <model/modules/logger.h>
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

  HashInfo info;

  info.chave = malloc(strlen(params[0]) + 1);
  strcpy(info.chave, params[0]);

  info.valor = malloc(strlen(params[1]) + 1);
  strcpy(info.valor, params[1]);

  HashTable_t.insert(controlador->tabelas[TIPO_X_DESCRICAO], info);

  // LOG_PRINT(LOG_FILE, "Tipo '%s' inserido", info.chave);

  return 1;
}