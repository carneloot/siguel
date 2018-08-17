#include "../comando.r"
#include "../controlador.r"

#include <figura.h>
#include <string.h>
#include <stdlib.h>
#include <utils.h>

int __comando_r(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;

  int id          = strtol(params[0], NULL, 10);
  char *cor_borda = params[1];
  char *cor       = params[2];

  Ponto2D size =
    Ponto2D_t.new(strtod(params[3], NULL), strtod(params[4], NULL));
  Ponto2D pos = Ponto2D_t.new(strtod(params[5], NULL), strtod(params[6], NULL));

  Figura figura = cria_retangulo(pos.x, pos.y, size.x, size.y, cor, cor_borda);
  set_id_figura(figura, id);

  Lista_t.insert((Lista) controlador->figuras, figura);

  size = Ponto2D_t.add(size, pos);

  controlador->max_geo.x = max(controlador->max_geo.x, size.x + 4);
  controlador->max_geo.y = max(controlador->max_geo.y, size.y + 4);

  return 1;
}