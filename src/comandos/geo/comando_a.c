#include <comando.r>
#include <controlador.r>

#include <stdlib.h>
#include <string.h>

#include <SVG.h>
#include <modules/logger.h>
#include <utils.h>
#include "../funcoes_checagem.h"

/**
 * Comando: a
 * Params:  i sufixo
 * Cria um arquivo svg contendo os círculos e
 * retângulos referentes aos comandos c e r
 * processados até o momento. Além dos
 * círculos e retângulos, devem ser traçadas
 * linhas a partir do centro de massa da figura
 * de identificador i até o centro de massa de
 * todas as outras figuras.
 * O nome do arquivo gerado deve ser
 * nomebase-sufixo.svg. As linhas devem ser
 * desenhadas usando a cor da borda da
 * figura i. Próximo a cada linha deve ser
 * escrito o seu comprimento.
 */
int comando_geo_a(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;

  int id = (int) strtol(params[0], NULL, 10);

  Posic_t posic;

  posic = lt_get_first(controlador->figuras);
  posic = lt_search(controlador->figuras, posic, &id, checar_id_figura);

  if (!posic) {
    LOG_ERRO(
      "Nao ha figura no id %d! (linha %d)", id, controlador->linha_atual);
    return 0;
  }

  Figura figura = lt_get(controlador->figuras, posic);

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

  Ponto2D_t pos, pos2;

  pos = get_centro_massa(figura);

  Posic_t iterator = lt_get_first(controlador->figuras);

  while (iterator) {
    Figura figura2 = lt_get(controlador->figuras, iterator);

    if (figura == figura2) {
      iterator = lt_get_next(controlador->figuras, iterator);
      continue;
    }

    pos2 = get_centro_massa(figura2);

    char *cor        = get_cor_borda(figura);
    double distancia = distancia_figuras(figura, figura2);

    desenha_linha(svg_saida, pos, pos2, 1, 1, cor, false);

    saida = format_string("%.1f", distancia);

    Ponto2D_t aux = p2d_add(pos, pos2);
    aux         = p2d_mult(aux, 0.5);
    aux.x += 10;

    escreve_texto(svg_saida, saida, aux, 15, cor);

    free(saida);

    iterator = lt_get_next(controlador->figuras, iterator);
  }

  salva_SVG(svg_saida);

  destruir_SVG(svg_saida);

  return 1;
}