#include <model/comando.r>
#include <control/controlador.r>

#include <model/mapa_viario/vertice.h>
#include <model/mapa_viario/aresta.h>
#include <model/utils.h>

#include <float.h>
#include <stdlib.h>
#include <stdbool.h>

double __distancia_vertice_ponto(const Item _vertice_info, const Item _ponto, int dim) {
  VerticeInfo vertice_info = (VerticeInfo) _vertice_info;

  Ponto2D ponto         = * (Ponto2D *) _ponto;
  Ponto2D ponto_vertice = vertice_info->pos;

  switch (dim) {
    case 0:  return mod(ponto.x - ponto_vertice.x);
    case 1:  return mod(ponto.y - ponto_vertice.y);
    case -1: return Ponto2D_t.dist_squared(ponto, ponto_vertice);
  }

  return DBL_MAX;
}

static VerticeInfo get_vertice_by_ponto(KDTree vertices, Ponto2D ponto) {
  return KDTree_t.nearest_neighbor(vertices, &ponto, __distancia_vertice_ponto).point1;
}

/**
 * Comando:    p?
 * Parametros: (t|p sufixo) (D|T) r1 r2 [cor]
 * Descricao:  Qual o melhor trajeto entre a origem que
 *             esta no registrador r1 e o destino que esta
 *             no registrador r2. Caso o percurso peça a
 *             representacao pictorica, o trajeto deve ser 
 *             pintado com a cor especificada
 *             ATENCAO: o calculo do percurso deve levar
 *             em consideracao as interdicoes
 *             estabelecidas pela ultima deteccao
 */

int comando_qry_p(void *_this, void *_controlador) {
  struct Comando *this = _this;
  struct Controlador *controlador = _controlador;

  return 1;
}

/**
 * Comando:    sp?
 * Parametros: (t|p sufixo) (D|T) n r1 ... rn [c1 c2]
 * Descricao:  Semelhante ao comando p? . O percurso
 *             deve começar na posição r1 e finalizar em
 *             rn, passando (na sequência) pelas posições
 *             determinadas pelo registradores r2...rn-1.
 *             c1 e c2 são duas cores que devem ser
 *             usadas alternadamente cada um dos trechos
 *             da sequência de caminhos. Por exemplo, o
 *             trecho de r1 à r2 deve ser pintado com a
 *             cor c1; o trecho de r2 a r3, com a cor c2;
 *             de r3 à r4, com a cor c1, e assim
 *             sucessivamente.
 */

int comando_qry_sp(void *_this, void *_controlador) {
  struct Comando *this = _this;
  struct Controlador *controlador = _controlador;

  // Se o primeiro parametro for igual a p, pictorica = true
  bool pictorica = (this->params[0][0] == 'p');
  bool distancia;

  char *sufixo, *cor[2];

  int total_registradores;

  if (pictorica) {
    sufixo = this->params[1];

    distancia = (this->params[2][0] == 'D');

    total_registradores = strtol(this->params[3], NULL, 10);

    cor[0] = this->params[3 + total_registradores];
    cor[1] = this->params[3 + total_registradores + 1];
  } else {
    distancia = (this->params[1][0] == 'D');

    total_registradores = strtol(this->params[2], NULL, 10);
  }

  for (int i = 0; i < total_registradores - 1; i++) {
    int r1 = strtol(this->params[3 + total_registradores + i]     + 1, NULL, 10);
    int r2 = strtol(this->params[3 + total_registradores + i + 1] + 1, NULL, 10);

    Ponto2D pos_origem  = controlador->registradores[r1];
    Ponto2D pos_destino = controlador->registradores[r2];

    VerticeInfo origem_info  = get_vertice_by_ponto(controlador->vertices_mapa_viario, pos_origem);
    VerticeInfo destino_info = get_vertice_by_ponto(controlador->vertices_mapa_viario, pos_destino);
  }

  return 1;
}