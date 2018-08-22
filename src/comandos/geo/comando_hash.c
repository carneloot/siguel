#include <comando.r>
#include <controlador.r>

#include <figura.h>
#include <string.h>
#include <stdlib.h>
#include <SVG.h>

int __comando_hash(void *_this, void *_controlador) {
  struct Controlador *controlador = (struct Controlador *) _controlador;

  /* Escrever SVG final */
  size_t length =
    strlen(controlador->nome_base) + strlen(controlador->dir_saida) + 5;

  char *saida = (char *) malloc(length * sizeof(char));

  sprintf(saida, "%s%s.svg", controlador->dir_saida, controlador->nome_base);

  SVG svg_saida = cria_SVG(saida, controlador->max_geo.x, controlador->max_geo.y);

  free(saida);

  desenhar_todas_figuras(controlador, svg_saida);

  desenhar_sobreposicoes(controlador, svg_saida);

  desenhar_elementos(controlador, svg_saida);

  salva_SVG(svg_saida);

  destruir_SVG(svg_saida);

  return 1;
}