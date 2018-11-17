#include <model/comando.r>
#include <control/controlador.r>

#include <stdlib.h>
#include <string.h>

#include <model/desenhavel.h>
#include <model/elemento.h>
#include <model/figura.h>
#include <model/modules/kdtree.h>
#include <model/utils.h>
#include "../funcoes_checagem.h"

static int elemento_dentro_figura(const void *_elemento, const void *_figura) {
  const Elemento elemento = (const Elemento) _elemento;
  const Figura figura     = (const Figura) _figura;

  int contem;

  // Se for quadra, checar a figura
  if (get_tipo_elemento(elemento) == QUADRA) {
    Figura figura_temp = get_figura_elemento(elemento);

    contem = dentro_figura(figura, figura_temp);

    destruir_figura(figura_temp);
  }

  // Se nao for quadra, comparar como ponto
  else {
    contem = contem_ponto(figura, get_pos(elemento));
  }

  return contem;
}

static Lista *comando_q_all(
  struct Comando *this,
  struct Controlador *controlador,
  Ponto2D pA, Ponto2D pB) {
  
  Lista *saida = calloc(4, sizeof(*saida));

  for (int i = 0; i < 4; i++) {
    KDTree arvore = controlador->elementos[i];

    if (KDTree_t.is_empty(arvore)) continue;

    Lista elementos = KDTree_t.range_search(arvore, elemento_dentro_rect, &pA, &pB);

    saida[i] = elementos;
  }

  return saida;
}

static void reportar_elementos(
  struct Comando *this, struct Controlador *controlador, Lista *elementos) {
  for (int i = 0; i < 4; i++) {
    Lista lista_atual = elementos[i];

    if (!lista_atual) continue;

    Posic iterator = Lista_t.get_first(lista_atual);

    while (iterator) {
      Elemento elemento = (Elemento) Lista_t.get(lista_atual, iterator);

      char *saida = get_info_elemento(elemento);
      strcat(saida, "\n");
      Lista_t.insert(controlador->saida, (Item) saida);

      iterator = Lista_t.get_next(lista_atual, iterator);
    }

    Lista_t.destruir(lista_atual, NULL);
  }
}

/**
 * Comando: q?
 * Params:  x y larg alt
 * Reporta as quadras e equipamentos
 * urbanos que estejam inteiramente dentro da
 * retângulo determinado pelos parâmetros do
 * comando.
 * Saída: arquivo.txt: todos os dados sobre as
 * quadras e equipamentos urbanos
 * selecionados.
 * arquivo.svg: traçar o retângulo da região
 * de busca com linhas pontilhadas.
 */
int comando_qzin(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;

  Ponto2D pos, size;
  Figura figura;

  pos  = Ponto2D_t.new(strtod(params[0], NULL), strtod(params[1], NULL));
  size = Ponto2D_t.new(strtod(params[2], NULL), strtod(params[3], NULL));

  figura = cria_retangulo(pos.x, pos.y, size.x, size.y, "transparent", "black");
  set_opacity_figura(figura, 0.8);
  set_dashed_figura(figura, FIG_BORDA_TRACEJADA);
  Lista_t.insert(controlador->saida_svg_qry,
    cria_desenhavel(figura, get_svg_figura, destruir_figura));

  Ponto2D new_max = Ponto2D_t.add(pos, size);
  new_max         = Ponto2D_t.add_scalar(new_max, 4);

  controlador->max_qry = Ponto2D_t.maximo(controlador->max_qry, new_max);

  char *prefixo = calloc(5, sizeof(char));
  strcpy(prefixo, "q?:\n");
  Lista_t.insert(controlador->saida, prefixo);

  Ponto2D pA = pos;
  Ponto2D pB = Ponto2D_t.add(pos, size);

  Lista *elementos = comando_q_all(this, controlador, pA, pB);

  // Checar se os elementos estão dentro da figura mesmo
  for (int i = 0; i < 4; i++) {
    Lista lista_atual = elementos[i];

    if (!lista_atual) continue;

    Posic iterator = Lista_t.get_first(lista_atual);

    while (iterator) {
      Posic next_iterator = Lista_t.get_next(lista_atual, iterator);
      Elemento elemento   = Lista_t.get(lista_atual, iterator);

      if (!elemento_dentro_figura(elemento, figura)) {
        Lista_t.remove(lista_atual, iterator);
      }

      iterator = next_iterator;
    }
  }

  reportar_elementos(this, controlador, elementos);

  free(elementos);

  return 1;
}

/**
 * Comando: Q?
 * Params:  raio x y
 * Similar a q?. A região de busca é dada pelo
 * círculo com centro x,y e raio raio.
 */
int comando_qzao(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;

  double r    = strtod(params[0], NULL);
  Ponto2D pos = Ponto2D_t.new(strtod(params[1], NULL), strtod(params[2], NULL));

  Figura figura = cria_circulo(pos.x, pos.y, r, "transparent", "black");
  set_opacity_figura(figura, 0.8);
  set_dashed_figura(figura, FIG_BORDA_TRACEJADA);

  Lista_t.insert(controlador->saida_svg_qry,
    cria_desenhavel(figura, get_svg_figura, destruir_figura));

  Ponto2D new_max = Ponto2D_t.add_scalar(pos, r + 4);

  controlador->max_qry = Ponto2D_t.maximo(controlador->max_qry, new_max);

  char *prefixo = calloc(5, sizeof(char));
  strcpy(prefixo, "Q?:\n");
  Lista_t.insert(controlador->saida, prefixo);

  Ponto2D pA = Ponto2D_t.sub_scalar(pos, r);
  Ponto2D pB = Ponto2D_t.add_scalar(pos, r);

  Lista *elementos = comando_q_all(this, controlador, pA, pB);

  // Checar se os elementos estão dentro da figura mesmo
  for (int i = 0; i < 4; i++) {
    Lista lista_atual = elementos[i];

    if (!lista_atual) continue;

    Posic iterator = Lista_t.get_first(lista_atual);

    while (iterator) {
      Posic next_iterator = Lista_t.get_next(lista_atual, iterator);
      Elemento elemento   = Lista_t.get(lista_atual, iterator);

      if (!elemento_dentro_figura(elemento, figura)) {
        Lista_t.remove(lista_atual, iterator);
      }

      iterator = next_iterator;
    }
  }

  reportar_elementos(this, controlador, elementos);

  free(elementos);

  return 1;
}