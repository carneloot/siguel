// Comando @xy
// Entrada: r x y
// Armazena a posição (x,y) no registrador r

#include <model/comando.r>
#include <control/controlador.r>

#include <string.h>
#include <stdlib.h>

#include <model/modules/ponto2d.h>

int comando_qry_xy(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;

  int numero_registrador    = strtol(params[0], NULL, 10);
  double x                  = strtod(params[1], NULL);
  double y                  = strtod(params[2], NULL);

  Ponto2D ponto = Ponto2D_t.new(x, y);
  controlador->registradores[numero_registrador] = ponto;

  return 1;
}