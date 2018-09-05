#include <comando.r>
#include <controlador.r>

#include <modules/logger.h>
#include <comercio.h>
#include <endereco.h>

#include <stdlib.h>

int __comando_ec_e(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;

  int num  = strtol(params[4], NULL, 10);  
  int face = char_to_face(params[3][0]);

  Comercio comercio = cria_comercio(
    params[0], params[1], params[2], face, num, params[5]);

  Lista_t.insert(controlador->comercios, comercio);

  return 1;
}