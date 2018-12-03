#include <comando.r>
#include <controlador.r>

#include <stdlib.h>
#include <string.h>

#include <elemento.h>
#include <figura.h>
#include <desenhavel.h>
#include <utils.h>
#include <modules/logger.h>
#include "../funcoes_checagem.h"

static Lista_t *comando_d_all(
  struct Comando *this,
  struct Controlador *controlador,
  Ponto2D_t pA, Ponto2D_t pB,
  const char *tipos) {
  int tipos_pesquisa[4] = {0, 0, 0, 0};

  for (int i = 0; i < strlen(tipos); i++) {
    switch (tipos[i]) {
      case 'q': tipos_pesquisa[QUADRA]     = 1; break;
      case 'h': tipos_pesquisa[HIDRANTE]   = 1; break;
      case 's': tipos_pesquisa[SEMAFORO]   = 1; break;
      case 'r': tipos_pesquisa[RADIO_BASE] = 1; break;
    }
  }
  
  Lista_t *saida = calloc(4, sizeof(*saida));
  
  // Procurar o elemento dentro da figura colocar no vetor de listas
  for (int i = 0; i < 4; i++) {
    if (!tipos_pesquisa[i])
      continue;
      
    KDTree_t arvore = controlador->elementos[i];
    
    if (kdt_is_empty(arvore)) continue;
    
    Lista_t elementos = kdt_range_search(arvore, elemento_dentro_rect, &pA, &pB);
    
    saida[i] = elementos;
  }

  return saida;
}

static void remover_elementos(
  struct Comando *this,
  struct Controlador *controlador,
  Lista_t *elementos) {

  // Remover quadras da HashTable
  if (elementos[QUADRA]) {
    Posic_t it = lt_get_first(elementos[QUADRA]);
    while (it) {
      Elemento elemento = lt_get(elementos[QUADRA], it);

      ht_remove(controlador->tabelas[CEP_X_QUADRA], get_cep_elemento(elemento));

      it = lt_get_next(elementos[QUADRA], it);
    }
  }

  if (elementos[RADIO_BASE]) {
    Posic_t it = lt_get_first(elementos[RADIO_BASE]);
    while (it) {
      Elemento elemento = lt_get(elementos[RADIO_BASE], it);

      ht_remove(controlador->tabelas[ID_X_RADIO], get_id_elemento(elemento));

      it = lt_get_next(elementos[RADIO_BASE], it);
    }
  }
  
  for (int h = 0; h < 4; h++) {
    if (!elementos[h]) continue;

    const char *tipo_elemento = get_tipo_string_elemento(h);

    Posic_t it = lt_get_first(elementos[h]);
    while (it) {
      Elemento elemento = lt_get(elementos[h], it);

      kdt_remove(controlador->elementos[h], elemento);
      char *cep   = get_cep_elemento(elemento);
      char *saida = format_string("%s: %s deletado (a).\n", tipo_elemento, cep);
      lt_insert(controlador->saida, saida);

      // destruir_elemento(elemento);

      it = lt_get_next(elementos[h], it);
    }
    
    lt_destroy(elementos[h], destruir_elemento);
  }

}

/**
 * Comando: dle
 * Params:  t x y larg alt
 * Semelhante ao dq. Remove equipamentos
 * urbanos do tipo t dentro da região.
 * (reporta o id e não mostra no .svg)
 * t pode ser qualquer combinação: h
 * (hidrante), s (semaforo), r (rádio-base)
 */
int comando_qry_dlezin(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;

  Ponto2D_t pos, size;

  pos  = p2d_new(strtod(params[1], NULL), strtod(params[2], NULL));
  size = p2d_new(strtod(params[3], NULL), strtod(params[4], NULL));

  Figura rect = cria_retangulo(pos.x, pos.y, size.x, size.y, "transparent", "black");
  set_opacity_figura(rect, 0.8);
  set_dashed_figura(rect, FIG_BORDA_TRACEJADA);
  
  lt_insert(controlador->saida_svg_qry,
    cria_desenhavel(rect, get_svg_figura, destruir_figura));

  Ponto2D_t new_max = p2d_add(pos, size);
  new_max         = p2d_add_scalar(new_max, 4);

  controlador->max_qry = p2d_maximo(controlador->max_qry, new_max);

  size = p2d_add(pos, size);

  Lista_t *elementos = comando_d_all(this, controlador, pos, size, params[0]);
  
  remover_elementos(this, controlador, elementos);
  
  free(elementos);
  
  return 1;
}

/**
 * Comando: Dle
 * Params:  t x y raio
 * Semelhante a dle. Remove os equipamentos
 * urbamos do tipo t dentro da região.
 * (Reporta o id, não mostra no .svg)
 */
int comando_qry_dlezao(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;

  Ponto2D_t pos = p2d_new(strtod(params[1], NULL), strtod(params[2], NULL));
  double r    = strtod(params[3], NULL);

  Figura circ = cria_circulo(pos.x, pos.y, r, "transparent", "black");
  set_opacity_figura(circ, 0.8);
  set_dashed_figura(circ, FIG_BORDA_TRACEJADA);

  lt_insert(controlador->saida_svg_qry,
    cria_desenhavel(circ, get_svg_figura, destruir_figura));
    
  Ponto2D_t pA = p2d_sub_scalar(pos, r);
  Ponto2D_t pB = p2d_add_scalar(pos, r);

  Ponto2D_t new_max = p2d_add_scalar(pos, r + 4);

  controlador->max_qry = p2d_maximo(controlador->max_qry, new_max);

  Lista_t *elementos = comando_d_all(this, controlador, pA, pB, params[0]);
  
  // Como eh no circulo, tem que remover os que nao estao no circulo
  for (int h = 1; h < 4; h++) {
    if (!elementos[h]) continue;
    
    Posic_t it = lt_get_first(elementos[h]);
    while (it) {
      Posic_t next_it = lt_get_next(elementos[h], it);
      Ponto2D_t posicao_elemento = get_pos(lt_get(elementos[h], it));
      
      // Se a figura estiver fora do circulo remover da lista
      if (p2d_dist_squared(pos, posicao_elemento) > sqr(r)) {
        lt_remove(elementos[h], it);
      }
      
      it = next_it;
    }

  }
  
  remover_elementos(this, controlador, elementos);
  
  free(elementos);
  
  return 1;
}

/**
 * Comando: dq
 * Params:  x y larg alt
 * Remove todas quadras que estiverem
 * inteiramente dentro do retângulo
 * determinado pelos parâmetros do comando.
 * Obs. Este comando, em particular, deve
 * remover uma quadra inserida pelo
 * comando q com idênticos parâmetros
 * x,y,larg e alt.
 * Saída. arquivo .txt: deve apresentar os ceps
 * das quadras removidas. arquivo .svg: deve
 * apresentar o retângulo correspondente à
 * região da consulta; as quadras removidas
 * não devem aparecer
 */
int comando_qry_dzin(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;

  Ponto2D_t pos, size;
  
  pos  = p2d_new(strtod(params[0], NULL), strtod(params[1], NULL));
  size = p2d_new(strtod(params[2], NULL), strtod(params[3], NULL));

  Figura figura = cria_retangulo(pos.x, pos.y, size.x, size.y, "transparent", "black");
  set_opacity_figura(figura, 0.8);
  set_dashed_figura(figura, FIG_BORDA_TRACEJADA);

  lt_insert(controlador->saida_svg_qry,
    cria_desenhavel(figura, get_svg_figura, destruir_figura));

  Ponto2D_t new_max = p2d_add(pos, size);
  new_max         = p2d_add_scalar(new_max, 4);

  controlador->max_qry = p2d_maximo(controlador->max_qry, new_max);

  size = p2d_add(pos, size);

  Lista_t *elementos = comando_d_all(this, controlador, pos, size, "q");

  // Checar se a quadra está inteiramente dentro da figura
  Lista_t quadras = elementos[0];
  Posic_t it      = (quadras) ? lt_get_first(quadras) : NULL;
  while (it) {
    Posic_t next_it   = lt_get_next(quadras, it);
    Elemento quadra = lt_get(quadras, it);

    Figura figura_quadra = get_figura_elemento(quadra);

    if (!dentro_figura(figura, figura_quadra))
      lt_remove(quadras, it);

    destruir_figura(figura_quadra);

    it = next_it;
  }

  remover_elementos(this, controlador, elementos);

  free(elementos);

  return 1;
}

/**
 * Comando: Dq
 * Params:  x y raio
 * Remove todas as quadras que estiverem
 * inteiramente contidas dentro do círculo de
 * centro em (x,y) e de raio raio.
 * Reporta no arquivo .txt o cep das quadras.
 * Quadras removidas não devem ser
 * mostradas no .svg.
 */
int comando_qry_dzao(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;

  Ponto2D_t pos;
  double r;
  
  r   = strtod(params[0], NULL);
  pos = p2d_new(strtod(params[1], NULL), strtod(params[2], NULL));

  Figura figura = cria_circulo(pos.x, pos.y, r, "transparent", "black");
  set_opacity_figura(figura, 0.8);
  set_dashed_figura(figura, FIG_BORDA_TRACEJADA);

  lt_insert(controlador->saida_svg_qry,
    cria_desenhavel(figura, get_svg_figura, destruir_figura));

  Ponto2D_t new_max = p2d_add_scalar(pos, r + 4);

  controlador->max_qry = p2d_maximo(controlador->max_qry, new_max);

  Ponto2D_t pA = p2d_sub_scalar(pos, r);
  Ponto2D_t pB = p2d_add_scalar(pos, r);

  Lista_t *elementos = comando_d_all(this, controlador, pA, pB, "q");

  // Checar se a quadra está inteiramente dentro da figura
  Lista_t quadras = elementos[0];
  Posic_t it      = (quadras) ? lt_get_first(quadras) : NULL;
  while (it) {
    Posic_t next_it   = lt_get_next(quadras, it);
    Elemento quadra = lt_get(quadras, it);

    Figura figura_quadra = get_figura_elemento(quadra);

    if (!dentro_figura(figura, figura_quadra))
      lt_remove(quadras, it);

    destruir_figura(figura_quadra);

    it = next_it;
  }

  remover_elementos(this, controlador, elementos);

  free(elementos);

  return 1;
}