#include <comando.r>
#include <controlador.r>

#include <mapa_viario/vertice.h>
#include <mapa_viario/aresta.h>

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

  Ponto2D_t pos = p2d_new(strtod(this->params[1], NULL), strtod(this->params[2], NULL));

  VerticeInfo vertice_info = create_vertice_info(pos, id);

  GrafoD_t.insert_vertice(controlador->mapa_viario, id);
  GrafoD_t.define_info_vertice(controlador->mapa_viario, id, vertice_info);

  kdt_insert(controlador->vertices_mapa_viario, vertice_info);

  Ponto2D_t new_max = p2d_add_scalar(pos, 10);
  controlador->max_qry = p2d_maximo(new_max, controlador->max_qry);

  return 1;
}

/**
 * Comando:    e
 * Parametros: i j ldir lesq cmp vm nome
 * Descricao:  cria a aresta (i,j) e associa as outras
 *             informações à aresta. Caso a aresta não
 *             possua quadras em algum de seus lados,
 *             esta ausência é indicada por um hífen (-)
 * 
 */
int comando_via_e(void *_this, void *_controlador) {
  struct Comando *this = _this;
  struct Controlador *controlador = _controlador;

  char *label_origem  = this->params[0];
  char *label_destino = this->params[1];

  char *quadra_direita  = this->params[2];
  char *quadra_esquerda = this->params[3];

  double comprimento      = strtod(this->params[4], NULL);
  double velocidade_media = strtod(this->params[5], NULL);

  char *nome_rua = this->params[6];

  VerticeInfo info_origem = GrafoD_t.get_info_vertice( controlador->mapa_viario, label_origem );
  VerticeInfo info_destino = GrafoD_t.get_info_vertice( controlador->mapa_viario, label_destino );

  Ponto2D_t pos_origem = info_origem->pos;
  Ponto2D_t pos_destino = info_destino->pos;

  Ponto2D_t pos_aresta = p2d_add( pos_origem, pos_destino );
  pos_aresta = p2d_mult( pos_aresta, 0.5 );
  

  ArestaInfo aresta_info = create_aresta_info(
    nome_rua, 
    label_origem, label_destino,
    quadra_direita, quadra_esquerda,
    comprimento, velocidade_media,
    pos_aresta
  );

  GrafoD_t.insert_aresta(controlador->mapa_viario, label_origem, label_destino);
  GrafoD_t.define_info_aresta(controlador->mapa_viario, label_origem, label_destino, aresta_info);

  kdt_insert(controlador->arestas_mapa_viario, aresta_info);

  return 1;
}