#include <comando.r>
#include <controlador.r>

#include <modules/logger.h>
#include <comercio/tipo_comercio.h>
#include <stdlib.h>
#include <string.h>

int __comando_ec_t(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  TipoComercio tipo_comercio = cria_tipo_comercio(
    this->params[0], this->params[1]);

  Lista_t.insert(controlador->tipos_comercio, tipo_comercio);

  HashInfo info = {
    .chave = tipo_comercio_identificacao(tipo_comercio),
    .valor = tipo_comercio_descricao(tipo_comercio)
  };

  HashTable_t.insert(controlador->tabelas[TIPO_X_DESCRICAO], info);

  // LOG_PRINT(LOG_FILE, "Tipo '%s' inserido", info.chave);

  return 1;
}