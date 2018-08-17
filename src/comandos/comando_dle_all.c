#include "../comando.r"
#include "../controlador.r"

#include <elemento.h>
#include <figura.h>
#include <stdlib.h>
#include <string.h>
#include <utils.h>
#include "funcoes_checagem.h"

// TODO: Implementar para KDTree
static int __comando_dle_all(
  struct Comando *this,
  struct Controlador *controlador,
  Figura figura,
  const char *tipos) {
  return 1;
  int tipos_pesquisa[4] = {0, 0, 0, 0};

  for (int i = 0; i < strlen(tipos); i++) {
    switch (tipos[i]) {
      case 'h': tipos_pesquisa[HIDRANTE] = 1; break;
      case 's': tipos_pesquisa[SEMAFORO] = 1; break;
      case 'r': tipos_pesquisa[RADIO_BASE] = 1; break;
    }
  }

  for (int i = 1; i < 4; i++) {
    if (!tipos_pesquisa[i])
      continue;

    Lista lista    = controlador->elementos[i];
    Posic iterator = Lista_t.get_first(lista);
    Posic next_it;

    iterator = Lista_t.search(lista, iterator, figura, elemento_dentro_figura);

    while (iterator) {
      next_it = Lista_t.get_next(lista, iterator);

      Elemento elemento = Lista_t.get(lista, iterator);

      Lista_t.remove(lista, iterator);

      char *info  = get_info_elemento(elemento);
      char *saida = calloc(12 + strlen(info), sizeof(char));
      sprintf(saida, "%s deletada.\n", info);
      Lista_t.insert(controlador->saida, (Item) saida);
      free(info);

      destruir_elemento(elemento);

      iterator = Lista_t.search(lista, next_it, figura, elemento_dentro_figura);
    }
  }

  Lista_t.insert(controlador->saida_svg_qry, (Item) figura);

  return 1;
}

int __comando_dlezin(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;

  Ponto2D pos, size;
  Figura figura;

  pos  = Ponto2D_t.new(strtod(params[1], NULL), strtod(params[2], NULL));
  size = Ponto2D_t.new(strtod(params[3], NULL), strtod(params[4], NULL));

  figura = cria_retangulo(pos.x, pos.y, size.x, size.y, "transparent", "black");

  Ponto2D new_max = Ponto2D_t.add(pos, size);
  new_max         = Ponto2D_t.add_scalar(new_max, 4);

  controlador->max_qry.x = max(controlador->max_qry.x, new_max.x);
  controlador->max_qry.y = max(controlador->max_qry.y, new_max.y);

  return __comando_dle_all(this, controlador, figura, params[0]);
}

int __comando_dlezao(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;

  Ponto2D pos = Ponto2D_t.new(strtod(params[1], NULL), strtod(params[2], NULL));
  double r    = strtod(params[3], NULL);

  Figura figura = cria_circulo(pos.x, pos.y, r, "transparent", "black");

  Ponto2D new_max = Ponto2D_t.add_scalar(pos, r + 4);

  controlador->max_qry.x = max(controlador->max_qry.x, new_max.x);
  controlador->max_qry.y = max(controlador->max_qry.y, new_max.y);

  return __comando_dle_all(this, controlador, figura, params[0]);
}