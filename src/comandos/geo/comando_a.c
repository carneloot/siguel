#include <comando.r>
#include <controlador.r>

#include <SVG.h>
#include <modules/logger.h>
#include <stdlib.h>
#include <string.h>
#include <utils.h>
#include "../funcoes_checagem.h"

int __comando_a(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;

  int id = (int) strtol(params[0], NULL, 10);

  Posic posic;

  posic = Lista_t.get_first(controlador->figuras);
  posic = Lista_t.search(controlador->figuras, posic, &id, checar_id_figura);

  if (!posic) {
    LOG_ERRO(
      "Nao ha figura no id %d! (linha %d)", id, controlador->linha_atual);
    return 0;
  }

  Figura figura = Lista_t.get(controlador->figuras, posic);

  /* Cria o sufixo padrao caso nao tenha sido especificado */
  char *sufixo = params[1];

  char *saida = format_string(
    "%s%s-%s.svg",
    controlador->dir_saida,
    controlador->nome_base,
    sufixo);

  SVG svg_saida =
    cria_SVG(saida, controlador->max_geo.x, controlador->max_geo.y);

  free(saida);

  desenhar_todas_figuras(controlador, svg_saida);

  Ponto2D pos, pos2;

  pos = get_centro_massa(figura);

  Posic iterator = Lista_t.get_first(controlador->figuras);

  while (iterator) {
    Figura figura2 = Lista_t.get(controlador->figuras, iterator);

    if (figura == figura2) {
      iterator = Lista_t.get_next(controlador->figuras, iterator);
      continue;
    }

    pos2 = get_centro_massa(figura2);

    char *cor        = get_cor_borda(figura);
    double distancia = distancia_figuras(figura, figura2);

    desenha_linha(svg_saida, pos, pos2, 1, cor);

    saida = format_string("%.1f", distancia);

    Ponto2D aux = Ponto2D_t.add(pos, pos2);
    aux         = Ponto2D_t.mult(aux, 0.5);
    aux.x += 10;

    escreve_texto(svg_saida, saida, aux, 15, cor);

    free(saida);

    iterator = Lista_t.get_next(controlador->figuras, iterator);
  }

  salva_SVG(svg_saida);

  destruir_SVG(svg_saida);

  return 1;
}