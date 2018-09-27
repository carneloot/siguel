#include <comando.r>
#include <controlador.r>

#include <figura.h>
#include <modules/logger.h>
#include <stdlib.h>
#include <utils.h>
#include <string.h>
#include "../funcoes_checagem.h"

int comando_d(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;

  int id1 = (int) strtol(params[0], NULL, 10);
  int id2 = (int) strtol(params[1], NULL, 10);

  Posic posic_figura1, posic_figura2;

  posic_figura1 = posic_figura2 = Lista_t.get_first(controlador->figuras);

  posic_figura1 =
    Lista_t.search(controlador->figuras, posic_figura1, &id1, checar_id_figura);
  if (!posic_figura1) {
    LOG_ERRO(
      "Nao ha figura no id %d! (linha %d)", id1, controlador->linha_atual);
    return 0;
  }

  posic_figura2 =
    Lista_t.search(controlador->figuras, posic_figura2, &id2, checar_id_figura);
  if (!posic_figura2) {
    LOG_ERRO(
      "Nao ha figura no id %d! (linha %d)", id2, controlador->linha_atual);
    return 0;
  }

  Figura figura1 = Lista_t.get(controlador->figuras, posic_figura1);
  Figura figura2 = Lista_t.get(controlador->figuras, posic_figura2);

  double distancia = distancia_figuras(figura1, figura2);

  char *saida = format_string("d %s %s\n%4.1f\n", params[0], params[1], distancia);

  Lista_t.insert(controlador->saida, (Item) saida);

  return 1;
}