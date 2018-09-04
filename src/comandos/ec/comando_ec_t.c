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

  HashInfo info = {
    .chave = "",
    .valor = tipo_comercio
  };

  info.chave = malloc(strlen(this->params[0]) + 1);
  strcpy(info.chave, this->params[0]);

  HashTable_t.insert(controlador->tipos_comercios, info);

  // LOG_PRINT(LOG_FILE, "Tipo '%s' inserido", info.chave);

  return 1;
}