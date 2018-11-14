#include <model/comando.r>
#include <control/controlador.r>

#include <model/mapa_viario/vertice.h>

#include <stdlib.h>

/**
 * Comando:    v
 * Parametros: id x y
 * Descricao:  cria o vertice id posicionado nas
 *             coordenadas [x,y]
 * 
 */

int comando_via_v(void *_this, void *_controlador) {
  struct Comando *this = _this;
  struct Controlador *controlador = _controlador;

  char *id = this->params[0];

  Ponto2D pos = Ponto2D_t.new(strtod(this->params[1], NULL), strtod(this->params[2], NULL));

  VerticeInfo vertice_info = create_vertice_info(pos, id);

  GrafoD_t.insert_vertice(controlador->mapa_viario, id);
  GrafoD_t.define_info_vertice(controlador->mapa_viario, id, vertice_info);

  return 1;
}