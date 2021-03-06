#include <comando.r>
#include <controlador.r>

#include <stdlib.h>
#include <string.h>

#include "../funcoes_checagem.h"

#include <model/colecao/kdtree.h>

#include <model/estrutural/desenhavel.h>

#include <model/trabalho/elemento.h>
#include <model/trabalho/figura.h>

#include <utils/utils.h>

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

static Lista_t *comando_q_all(
  struct Comando *this,
  struct Controlador *controlador,
  Ponto2D_t pA, Ponto2D_t pB) {
  
  Lista_t *saida = calloc(4, sizeof(*saida));

  for (int i = 0; i < 4; i++) {
    KDTree_t arvore = controlador->elementos[i];

    if (kdt_is_empty(arvore)) continue;

    Lista_t elementos = kdt_range_search(arvore, elemento_dentro_rect, &pA, &pB);

    saida[i] = elementos;
  }

  return saida;
}

static void reportar_elementos(
  struct Comando *this, struct Controlador *controlador, Lista_t *elementos) {
  for (int i = 0; i < 4; i++) {
    Lista_t lista_atual = elementos[i];

    if (!lista_atual) continue;

    Posic_t iterator = lt_get_first(lista_atual);

    while (iterator) {
      Elemento elemento = (Elemento) lt_get(lista_atual, iterator);

      char *saida = get_info_elemento(elemento);
      strcat(saida, "\n");
      lt_insert(controlador->saida, (void *) saida);

      iterator = lt_get_next(lista_atual, iterator);
    }

    lt_destroy(lista_atual, NULL);
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
int comando_qry_qzin(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;

  Ponto2D_t pos, size;
  Figura figura;

  pos  = p2d_new(strtod(params[0], NULL), strtod(params[1], NULL));
  size = p2d_new(strtod(params[2], NULL), strtod(params[3], NULL));

  figura = cria_retangulo(pos.x, pos.y, size.x, size.y, "transparent", "black");
  set_opacity_figura(figura, 0.8);
  set_dashed_figura(figura, FIG_BORDA_TRACEJADA);
  lt_insert(controlador->saida_svg_qry,
    cria_desenhavel(figura, get_svg_figura, destruir_figura));

  Ponto2D_t new_max = p2d_add(pos, size);
  new_max         = p2d_add_scalar(new_max, 4);

  controlador->max_qry = p2d_maximo(controlador->max_qry, new_max);

  char *prefixo = calloc(5, sizeof(char));
  strcpy(prefixo, "q?:\n");
  lt_insert(controlador->saida, prefixo);

  Ponto2D_t pA = pos;
  Ponto2D_t pB = p2d_add(pos, size);

  Lista_t *elementos = comando_q_all(this, controlador, pA, pB);

  // Checar se os elementos estão dentro da figura mesmo
  for (int i = 0; i < 4; i++) {
    Lista_t lista_atual = elementos[i];

    if (!lista_atual) continue;

    Posic_t iterator = lt_get_first(lista_atual);

    while (iterator) {
      Posic_t next_iterator = lt_get_next(lista_atual, iterator);
      Elemento elemento   = lt_get(lista_atual, iterator);

      if (!elemento_dentro_figura(elemento, figura)) {
        lt_remove(lista_atual, iterator);
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
int comando_qry_qzao(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;

  double r    = strtod(params[0], NULL);
  Ponto2D_t pos = p2d_new(strtod(params[1], NULL), strtod(params[2], NULL));

  Figura figura = cria_circulo(pos.x, pos.y, r, "transparent", "black");
  set_opacity_figura(figura, 0.8);
  set_dashed_figura(figura, FIG_BORDA_TRACEJADA);

  lt_insert(controlador->saida_svg_qry,
    cria_desenhavel(figura, get_svg_figura, destruir_figura));

  Ponto2D_t new_max = p2d_add_scalar(pos, r + 4);

  controlador->max_qry = p2d_maximo(controlador->max_qry, new_max);

  char *prefixo = calloc(5, sizeof(char));
  strcpy(prefixo, "Q?:\n");
  lt_insert(controlador->saida, prefixo);

  Ponto2D_t pA = p2d_sub_scalar(pos, r);
  Ponto2D_t pB = p2d_add_scalar(pos, r);

  Lista_t *elementos = comando_q_all(this, controlador, pA, pB);

  // Checar se os elementos estão dentro da figura mesmo
  for (int i = 0; i < 4; i++) {
    Lista_t lista_atual = elementos[i];

    if (!lista_atual) continue;

    Posic_t iterator = lt_get_first(lista_atual);

    while (iterator) {
      Posic_t next_iterator = lt_get_next(lista_atual, iterator);
      Elemento elemento   = lt_get(lista_atual, iterator);

      if (!elemento_dentro_figura(elemento, figura)) {
        lt_remove(lista_atual, iterator);
      }

      iterator = next_iterator;
    }
  }

  reportar_elementos(this, controlador, elementos);

  free(elementos);

  return 1;
}