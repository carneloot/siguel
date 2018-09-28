#include <model/comando.r>
#include <control/controlador.r>

#include <string.h>
#include <stdlib.h>

#include <model/figura.h>
#include <model/utils.h>

int comando_r(void *_this, void *_controlador) {
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

  Ponto2D new_max = Ponto2D_t.add(pos, size);
  new_max = Ponto2D_t.add_scalar(new_max, 4);

  controlador->max_geo = Ponto2D_t.maximo(controlador->max_geo, new_max);

  return 1;
}