#include <comando.r>
#include <controlador.r>

#include <figura.h>
#include <string.h>
#include <stdlib.h>
#include <SVG.h>
#include <utils.h>

int comando_hash(void *_this, void *_controlador) {
  struct Controlador *controlador = (struct Controlador *) _controlador;

  /* Escrever SVG final */
  char *saida = format_string("%s%s.svg", controlador->dir_saida, controlador->nome_base);

  SVG svg_saida = cria_SVG(saida, controlador->max_geo.x, controlador->max_geo.y);

  free(saida);

  desenhar_todas_figuras(controlador, svg_saida);

  desenhar_sobreposicoes(controlador, svg_saida);

  desenhar_elementos(controlador, svg_saida);

  salva_SVG(svg_saida);

  destruir_SVG(svg_saida);

  return 1;
}