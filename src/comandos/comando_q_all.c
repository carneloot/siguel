#include "../comando.r"
#include "../controlador.r"

#include <elemento.h>
#include <figura.h>
#include <stdlib.h>
#include <string.h>
#include <utils.h>
#include "funcoes_checagem.h"

// TODO: Implementar para KDTree
static int __comando_q_all(
  struct Comando *this,
  struct Controlador *controlador,
  Figura figura,
  const char *prefixo) {
    return 1;
  Lista_t.insert(controlador->saida, (Item) prefixo);

  for (int i = 0; i < 4; i++) {
    Lista lista_atual = controlador->elementos[i];

    Posic iterator = Lista_t.get_first(lista_atual);

    iterator =
      Lista_t.search(lista_atual, iterator, figura, elemento_dentro_figura);

    while (iterator) {
      Elemento elemento = (Elemento) Lista_t.get(lista_atual, iterator);

      char *saida = get_info_elemento(elemento);
      strcat(saida, "\n");
      Lista_t.insert(controlador->saida, (Item) saida);

      iterator = Lista_t.search(
        lista_atual,
        Lista_t.get_next(lista_atual, iterator),
        figura,
        elemento_dentro_figura);
    }
  }

  Lista_t.insert(controlador->saida_svg_qry, (Item) figura);

  return 1;
}

int __comando_qzin(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;

  Ponto2D pos, size;
  Figura figura;

  pos  = Ponto2D_t.new(strtod(params[0], NULL), strtod(params[1], NULL));
  size = Ponto2D_t.new(strtod(params[2], NULL), strtod(params[3], NULL));

  figura = cria_retangulo(pos.x, pos.y, size.x, size.y, "transparent", "black");

  Ponto2D new_max = Ponto2D_t.add(pos, size);
  new_max         = Ponto2D_t.add_scalar(new_max, 4);

  controlador->max_qry.x = max(controlador->max_qry.x, new_max.x);
  controlador->max_qry.y = max(controlador->max_qry.y, new_max.y);

  char *prefixo = calloc(5, sizeof(char));
  strcpy(prefixo, "q?:\n");

  return __comando_q_all(this, controlador, figura, prefixo);
}

int __comando_qzao(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;

  double r    = strtod(params[0], NULL);
  Ponto2D pos = Ponto2D_t.new(strtod(params[1], NULL), strtod(params[2], NULL));

  Figura figura = cria_circulo(pos.x, pos.y, r, "transparent", "black");

  Ponto2D new_max = Ponto2D_t.add_scalar(pos, r + 4);

  controlador->max_qry.x = max(controlador->max_qry.x, new_max.x);
  controlador->max_qry.y = max(controlador->max_qry.y, new_max.y);

  char *prefixo = calloc(5, sizeof(char));
  strcpy(prefixo, "Q?:\n");

  return __comando_q_all(this, controlador, figura, prefixo);
}