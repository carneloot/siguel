#include <comando.r>
#include <controlador.r>

#include <stdlib.h>
#include <elemento.h>
#include <utils.h>
#include <math.h>
#include <desenhavel.h>

#include "svg_linha.h"

static double distancia(void * const _a, void * const _b, int dim) {
  const Ponto2D *a = _a;
  const Ponto2D *b = _b;

  switch (dim) {
    case -1: return Ponto2D_t.dist_squared(*a, *b);
    case  0: return sqr(a->x - b->x);
    case  1: return sqr(a->y - b->y);
  }

  return 0;
}

static Pair hidrante_mais_proximo(
  struct Controlador *controlador, Ponto2D *ponto) {
  return KDTree_t.nearest_neighbor(controlador->elementos[HIDRANTE],
    ponto, distancia);
}

int __comando_qry_hmp(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char *id = this->params[0];

  Elemento radio_base = HashTable_t.get(controlador->tabelas[ID_X_RADIO], id).valor;

  Pair pair = hidrante_mais_proximo(controlador, radio_base);
  pair.distance = sqrt(pair.distance);

  char *info_radio    = get_info_elemento(radio_base);
  char *info_hidrante = get_info_elemento(pair.point1);

  char *saida = format_string(
    "Radio Base mais proxima do Hidrante:\n\t%s\n\t%s\n\tDistancia: %.2f\n",
    info_radio, info_hidrante, pair.distance
  );

  Lista_t.insert(controlador->saida, saida);

  free(info_hidrante);
  free(info_radio);

  // Adicionar linha
  Ponto2D a = *((Ponto2D *) radio_base);
  Ponto2D b = *((Ponto2D *) pair.point1);
  Seta pontos = cria_svg_pontos(a, b, "aqua", 0, 1);
  Lista_t.insert(controlador->saida_svg_qry, cria_desenhavel(
    pontos, svg_pontos, free_svg_pontos));

  return 1;
}