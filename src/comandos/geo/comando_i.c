#include <comando.r>
#include <controlador.r>

#include <string.h>
#include <stdlib.h>

#include <figura.h>
#include <modules/logger.h>
#include <utils.h>
#include "../funcoes_checagem.h"

/**
 * Comando: i
 * Params:  j x y
 * O ponto (x,y) é interno à j-ésima forma 
 * geométrica?7
 */
int comando_geo_i(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;

  int id      = (int) strtol(params[0], NULL, 10);
  Ponto2D_t pos = p2d_new(strtod(params[1], NULL), strtod(params[2], NULL));

  Posic_t posic_figura1 = lt_get_first(controlador->figuras);
  posic_figura1 =
    lt_search(controlador->figuras, posic_figura1, &id, checar_id_figura);

  if (!posic_figura1) {
    LOG_ERRO(
      "Nao ha figura no id %d! (linha %d)", id, controlador->linha_atual);
    return 0;
  }

  Figura figura1 = lt_get(controlador->figuras, posic_figura1);

  char *saida;

  if (contem_ponto(figura1, pos))
    saida = format_string("i %s %s %s\nSIM\n", params[0], params[1], params[2]);
  else
    saida = format_string("i %s %s %s\nNAO\n", params[0], params[1], params[2]);

  lt_insert(controlador->saida, (Item) saida);

  return 1;
}