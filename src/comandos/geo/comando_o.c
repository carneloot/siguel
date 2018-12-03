#include <comando.r>
#include <controlador.r>

#include <string.h>
#include <stdlib.h>

#include <figura.h>
#include <modules/logger.h>
#include <utils.h>
#include "../funcoes_checagem.h"

/**
 * Comando: o
 * Params:  j k
 * As formas geométricas cujos
 * indentificadores são j e k se sobrepõem?
 */
int comando_geo_o(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;

  int id1 = (int) strtol(params[0], NULL, 10);
  int id2 = (int) strtol(params[1], NULL, 10);

  Posic_t posic_figura1, posic_figura2;

  posic_figura1 = posic_figura2 = lt_get_first(controlador->figuras);

  // Checa se as figuras estao na lista de figuras
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

  char *saida;

  if (sobrepoe_figura(figura1, figura2)) {
    saida = format_string("o %s %s\nSIM\n", params[0], params[1]);
    /* Desenhar retangulo no lugar da sobreposicao */
    lt_insert(
      controlador->sobreposicoes,
      (Item) get_rect_sobreposicao(figura1, figura2, "purple"));
  } else
    saida = format_string("o %s %s\nNAO\n", params[0], params[1]);

  lt_insert(controlador->saida, (Item) saida);

  return 1;
}