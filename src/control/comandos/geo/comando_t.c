#include <model/comando.r>
#include <control/controlador.r>

#include <string.h>
#include <stdlib.h>

#include <model/elemento.h>
#include <model/utils.h>

int comando_t(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;

  char *cep   = params[0];
  Ponto2D pos = Ponto2D_t.new(strtod(params[1], NULL), strtod(params[2], NULL));

  Elemento elemento = cria_radio_base(pos.x, pos.y, cep);

  set_cor_elemento(elemento, controlador->cores[RADIO_BASE]);
  set_cor_borda_elemento(elemento, controlador->cores_borda[RADIO_BASE]);

  KDTree_t.insert(controlador->elementos[RADIO_BASE], elemento);

  HashInfo info = {
    .chave = get_id_elemento(elemento),
    .valor = elemento
  };
  HashTable_t.insert(controlador->tabelas[ID_X_RADIO], info);

  Ponto2D new_max = Ponto2D_t.add_scalar(pos, RAIO_EQUIPAMENTOS + 4);

  controlador->max_geo = Ponto2D_t.maximo(controlador->max_geo, new_max);

  controlador->max_qry = Ponto2D_t.maximo(controlador->max_qry, new_max);

  return 1;
}