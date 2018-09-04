#include <comando.r>
#include <controlador.r>

#include <modules/logger.h>

int __comando_ec_e(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  return 1;
}