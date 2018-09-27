#include <comando.r>
#include <controlador.r>

#include <elemento.h>
#include <string.h>
#include <stdlib.h>
#include <utils.h>

int comando_s(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;

  char *cep   = params[0];
  Ponto2D pos = Ponto2D_t.new(strtod(params[1], NULL), strtod(params[2], NULL));

  Elemento elemento = cria_semaforo(pos.x, pos.y, cep);

  set_cor_elemento(elemento, controlador->cores[SEMAFORO]);
  set_cor_borda_elemento(elemento, controlador->cores_borda[SEMAFORO]);

  KDTree_t.insert(controlador->elementos[SEMAFORO], elemento);

  Ponto2D new_max = Ponto2D_t.add_scalar(pos, RAIO_EQUIPAMENTOS + 4);

  controlador->max_geo = Ponto2D_t.maximo(controlador->max_geo, new_max);
  controlador->max_qry = Ponto2D_t.maximo(controlador->max_qry, new_max);

  return 1;
}