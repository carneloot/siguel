#include "svg_custom.h"

#include <stdlib.h>

#include <utils.h>

void *cria_custom(Ponto2D pos, double tamanho, char *string, char *cor) {
  struct Custom *custom = malloc(sizeof(*custom));

  custom->pos     = pos;
  custom->tamanho = tamanho;
  custom->string  = format_string(string);
  custom->cor     = (cor) ? format_string(cor) : NULL;

  return custom;
}

char *print_custom_asset(void *_custom) {
  struct Custom *custom = _custom;
  char *saida = format_string(
    "<svg width=\"%.2lf\" height=\"%.2lf\" x=\"%.2lf\" y=\"%.2lf\">\n%s</svg>\n",
    custom->tamanho, custom->tamanho,
    custom->pos.x, custom->pos.y,
    custom->string
  );
  return saida;
}

char *print_custom_texto(void *_custom) {
  struct Custom *custom = _custom;
  char *saida = format_string(
    "<text x=\"%.1f\" y=\"%.1f\" "
    "style=\"fill:%s;font-size:%.1fpx;font-family:sans-serif\">%s</text>\n",
    custom->pos.x, custom->pos.y,
    custom->cor, custom->tamanho, custom->string
  );
  return saida;
}

void free_custom(void *_custom) {
  struct Custom *custom = _custom;
  if (custom->cor)
    free(custom->cor);
  free(custom->string);
  free(custom);
}