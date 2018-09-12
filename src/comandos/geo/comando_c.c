#include <comando.r>
#include <controlador.r>

#include <figura.h>
#include <string.h>
#include <stdlib.h>
#include <utils.h>

int __comando_c(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;

  int id          = strtol(params[0], NULL, 10);
  char *cor_borda = params[1];
  char *cor       = params[2];
  double r        = strtod(params[3], NULL);
  Ponto2D pos = Ponto2D_t.new(strtod(params[4], NULL), strtod(params[5], NULL));

  Figura figura = cria_circulo(pos.x, pos.y, r, cor, cor_borda);
  set_id_figura(figura, id);

  Lista_t.insert((Lista) controlador->figuras, figura);
  
  Ponto2D new_max = Ponto2D_t.add_scalar(pos, r + 4);

  controlador->max_geo = Ponto2D_t.maximo(controlador->max_geo, new_max);
  
  return 1;
}