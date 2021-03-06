#include <comando.r>
#include <controlador.r>

#include <stdlib.h>
#include <string.h>

#include "../funcoes_checagem.h"

#include <model/trabalho/figura.h>

#include <utils/logger.h>
#include <utils/utils.h>

/**
 * Comando: d
 * Params:  i j
 * Qual é a distância entre os centro de massa
 * das formas geométricas i e j?
 */
int comando_geo_d(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;

  int id1 = (int) strtol(params[0], NULL, 10);
  int id2 = (int) strtol(params[1], NULL, 10);

  Posic_t posic_figura1, posic_figura2;

  posic_figura1 = posic_figura2 = lt_get_first(controlador->figuras);

  posic_figura1 =
    lt_search(controlador->figuras, posic_figura1, &id1, checar_id_figura);
  if (!posic_figura1) {
    LOG_ERRO(
      "Nao ha figura no id %d! (linha %d)", id1, controlador->linha_atual);
    return 0;
  }

  posic_figura2 =
    lt_search(controlador->figuras, posic_figura2, &id2, checar_id_figura);
  if (!posic_figura2) {
    LOG_ERRO(
      "Nao ha figura no id %d! (linha %d)", id2, controlador->linha_atual);
    return 0;
  }

  Figura figura1 = lt_get(controlador->figuras, posic_figura1);
  Figura figura2 = lt_get(controlador->figuras, posic_figura2);

  double distancia = distancia_figuras(figura1, figura2);

  char *saida = format_string("d %s %s\n%4.1f\n", params[0], params[1], distancia);

  lt_insert(controlador->saida, (void *) saida);

  return 1;
}