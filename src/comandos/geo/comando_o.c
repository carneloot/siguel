#include <comando.r>
#include <controlador.r>
#include "../funcoes_checagem.h"

#include <figura.h>
#include <modules/logger.h>
#include <stdlib.h>
#include <string.h>

int __comando_o(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;

  int id1 = (int) strtol(params[0], NULL, 10);
  int id2 = (int) strtol(params[1], NULL, 10);

  Posic posic_figura1, posic_figura2;

  posic_figura1 = posic_figura2 = Lista_t.get_first(controlador->figuras);

  // Checa se as figuras estao na lista de figuras
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

  size_t length = 9 + strlen(params[0]) + strlen(params[1]);

  char *saida = (char *) malloc(length * sizeof(char));

  if (sobrepoe_figura(figura1, figura2)) {
    sprintf(saida, "o %s %s\nSIM\n", params[0], params[1]);
    /* Desenhar retangulo no lugar da sobreposicao */
    Lista_t.insert(
      controlador->sobreposicoes,
      (Item) get_rect_sobreposicao(figura1, figura2));
  } else
    sprintf(saida, "o %s %s\nNAO\n", params[0], params[1]);

  Lista_t.insert(controlador->saida, (Item) saida);

  return 1;
}