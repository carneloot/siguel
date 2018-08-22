#include <comando.r>
#include <controlador.r>

#include <elemento.h>
#include <figura.h>
#include <stdlib.h>
#include <string.h>
#include <utils.h>
#include "../funcoes_checagem.h"

// TODO: Implementar para KDTree
static int __comando_dq_all(
  struct Comando *this, struct Controlador *controlador, Figura figura) {
  return 1;
  Lista lista    = controlador->elementos[QUADRA];
  Posic iterator = Lista_t.get_first(lista);
  Posic next_it;

  iterator = Lista_t.search(lista, iterator, figura, elemento_dentro_figura);
  while (iterator) {
    next_it = Lista_t.get_next(lista, iterator);

    Elemento elemento = Lista_t.get(lista, iterator);

    Lista_t.remove(lista, iterator);

    char *cep   = get_cep_elemento(elemento);
    char *saida = malloc(20 + strlen(cep));
    sprintf(saida, "Quadra: %s deletada.\n", cep);
    Lista_t.insert(controlador->saida, saida);

    destruir_elemento(elemento);

    iterator = Lista_t.search(lista, next_it, figura, elemento_dentro_figura);
  }

  Lista_t.insert(controlador->saida_svg_qry, figura);

  return 1;
}

int __comando_dzin(void *_this, void *_controlador) {
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

  return __comando_dq_all(this, controlador, figura);
}

int __comando_dzao(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;

  double r    = strtod(params[0], NULL);
  Ponto2D pos = Ponto2D_t.new(strtod(params[1], NULL), strtod(params[2], NULL));

  Figura figura = cria_circulo(pos.x, pos.y, r, "transparent", "black");

  Ponto2D new_max = Ponto2D_t.add_scalar(pos, r + 4);

  controlador->max_qry.x = max(controlador->max_qry.x, new_max.x);
  controlador->max_qry.y = max(controlador->max_qry.y, new_max.y);

  return __comando_dq_all(this, controlador, figura);
}