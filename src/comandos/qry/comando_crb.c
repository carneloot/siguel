#include <comando.r>
#include <controlador.r>

#include <math.h>
#include <string.h>
#include <stdlib.h>

#include <elemento.h>
#include <utils.h>
#include <desenhavel.h>

#define RAIO_RADIOS_PROXIMOS 25

double distanciaElementos(const Item _this, const Item _other, int dim) {
  struct Elemento *this  = (struct Elemento *) _this;
  struct Elemento *other = (struct Elemento *) _other;

  double result;

  switch (dim) {
    case -1:
      result = p2d_dist_squared(get_pos(this), get_pos(other));
      break;
    case 0: result = sqr(get_x(this) - get_x(other)); break;
    case 1: result = sqr(get_y(this) - get_y(other)); break;
  }

  return result;
}

/**
 * Comando: crb?
 * Determina quais são as duas rádio-bases
 * mais próximas. Saida: arquivo.svg: circular
 * a rádio-base destacando-a; arquivo.txt:
 * reportar id das torres e a distância.
 */
int comando_qry_crb(void *_this, void *_controlador) {
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char *saida;

  // Se nao houver torres
  if (KDTree_t.is_empty(controlador->elementos[RADIO_BASE])) {
    saida = format_string(
      "Nao ha torres de celular suficientes para checar a distancia.\n");
    Lista_t.insert(controlador->saida, saida);
    return 1;
  }

  Pair pair = KDTree_t.closest_pair(
    controlador->elementos[RADIO_BASE], distanciaElementos);

  Elemento radio1, radio2;
  double distancia;

  radio1    = pair.point1;
  radio2    = pair.point2;
  distancia = sqrt(pair.distance);

  char *id1 = get_id_elemento(radio1);
  char *id2 = get_id_elemento(radio2);

  saida = format_string(
    "Torres de radio mais proximas:\n%s e %s. Distancia: %3.2f\n",
    id1,
    id2,
    distancia);

  Lista_t.insert(controlador->saida, saida);

  // Desenhar os circulos nas torres mais proximas
  Ponto2D_t pos;

  pos = get_pos(radio1);

  for (int i = 0; i < 2; i++) {
    Figura circ = cria_circulo(
        pos.x, pos.y, RAIO_RADIOS_PROXIMOS, "transparent", "purple");

    set_opacity_figura(circ, 0.8);
    set_dashed_figura(circ, FIG_BORDA_TRACEJADA);

    Lista_t.insert(
      controlador->saida_svg_qry,
      cria_desenhavel(circ, get_svg_figura, destruir_figura));

    Ponto2D_t new_max = p2d_add_scalar(pos, RAIO_RADIOS_PROXIMOS);
    
    controlador->max_qry = p2d_maximo(controlador->max_qry, new_max);

    pos = get_pos(radio2);
  }

  return 1;
}