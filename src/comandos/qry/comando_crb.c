#include <comando.r>
#include <controlador.r>

#include <elemento.h>
#include <utils.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#define RAIO_RADIOS_PROXIMOS 25

double distanciaElementos(const Item _this, const Item _other, int dim) {
  struct Elemento *this  = (struct Elemento *) _this;
  struct Elemento *other = (struct Elemento *) _other;

  double result;

  switch (dim) {
    case -1:
      result = Ponto2D_t.dist_squared(get_pos(this), get_pos(other));
      break;
    case 0: result = sqr(get_x(this) - get_x(other)); break;
    case 1: result = sqr(get_y(this) - get_y(other)); break;
  }

  return result;
}

int __comando_crb(void *_this, void *_controlador) {
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char *saida;

  // Se nao houver torres
  if (KDTree_t.is_empty(controlador->elementos[RADIO_BASE])) {
    saida = malloc(63);
    sprintf(
      saida, "Nao ha torres de celular suficientes para checar a distancia.\n");
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

  size_t length = 49 + strlen(id1) + strlen(id2) + 6;

  saida = malloc(length);

  sprintf(
    saida,
    "Torres de radio mais proximas:\n%s e %s. Distancia: %3.2f\n",
    id1,
    id2,
    distancia);

  Lista_t.insert(controlador->saida, saida);

  // Desenhar os circulos nas torres mais proximas
  Ponto2D pos, new_max;

  pos = get_pos(radio1);

  for (int i = 0; i < 2; i++) {
    Lista_t.insert(
      controlador->saida_svg_qry,
      cria_circulo(
        pos.x, pos.y, RAIO_RADIOS_PROXIMOS, "transparent", "purple"));

    new_max                = Ponto2D_t.add_scalar(pos, RAIO_RADIOS_PROXIMOS);
    controlador->max_qry.x = max(controlador->max_qry.x, new_max.x);
    controlador->max_qry.y = max(controlador->max_qry.y, new_max.y);

    pos = get_pos(radio2);
  }

  return 1;
}