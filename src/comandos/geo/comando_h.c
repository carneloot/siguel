#include <comando.r>
#include <controlador.r>

#include <elemento.h>
#include <string.h>
#include <stdlib.h>
#include <utils.h>

int __comando_h(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;

  char *cep   = params[0];
  Ponto2D pos = Ponto2D_t.new(strtod(params[1], NULL), strtod(params[2], NULL));

  Elemento elemento = cria_hidrante(pos.x, pos.y, cep);

  set_cor_elemento(elemento, controlador->cores[HIDRANTE]);
  set_cor_borda_elemento(elemento, controlador->cores_borda[HIDRANTE]);

  KDTree_t.insert(controlador->elementos[HIDRANTE], elemento);

  Ponto2D new_max = Ponto2D_t.add_scalar(pos, RAIO_EQUIPAMENTOS + 4);

  controlador->max_geo.x = max(controlador->max_geo.x, new_max.x);
  controlador->max_geo.y = max(controlador->max_geo.y, new_max.y);

  controlador->max_qry.x = max(controlador->max_qry.x, new_max.x);
  controlador->max_qry.y = max(controlador->max_qry.y, new_max.y);

  return 1;
}