#include <model/comando.r>
#include <control/controlador.r>

#include <stdlib.h>
#include <string.h>

#include <model/elemento.h>
#include <model/figura.h>
#include <model/desenhavel.h>
#include <model/utils.h>
#include <model/modules/logger.h>
#include "../funcoes_checagem.h"

static Lista *comando_d_all(
  struct Comando *this,
  struct Controlador *controlador,
  Ponto2D pA, Ponto2D pB,
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
  
  Lista *saida = calloc(4, sizeof(*saida));
  
  // Procurar o elemento dentro da figura colocar no vetor de listas
  for (int i = 0; i < 4; i++) {
    if (!tipos_pesquisa[i])
      continue;
      
    KDTree arvore = controlador->elementos[i];
    
    if (KDTree_t.is_empty(arvore)) continue;
    
    Lista elementos = KDTree_t.range_search(arvore, elemento_dentro_rect, &pA, &pB);
    
    saida[i] = elementos;
  }

  return saida;
}

static void remover_elementos(
  struct Comando *this,
  struct Controlador *controlador,
  Lista *elementos) {

  // Remover quadras da HashTable
  if (elementos[QUADRA]) {
    Posic it = Lista_t.get_first(elementos[QUADRA]);
    while (it) {
      Elemento elemento = Lista_t.get(elementos[QUADRA], it);

      HashTable_t.remove(controlador->tabelas[CEP_X_QUADRA], get_cep_elemento(elemento));

      it = Lista_t.get_next(elementos[QUADRA], it);
    }
  }

  if (elementos[RADIO_BASE]) {
    Posic it = Lista_t.get_first(elementos[RADIO_BASE]);
    while (it) {
      Elemento elemento = Lista_t.get(elementos[RADIO_BASE], it);

      HashTable_t.remove(controlador->tabelas[ID_X_RADIO], get_id_elemento(elemento));

      it = Lista_t.get_next(elementos[RADIO_BASE], it);
    }
  }
  
  for (int h = 0; h < 4; h++) {
    if (!elementos[h]) continue;

    const char *tipo_elemento = get_tipo_string_elemento(h);

    Posic it = Lista_t.get_first(elementos[h]);
    while (it) {
      Elemento elemento = Lista_t.get(elementos[h], it);

      KDTree_t.remove(controlador->elementos[h], elemento);
      char *cep   = get_cep_elemento(elemento);
      char *saida = format_string("%s: %s deletado (a).\n", tipo_elemento, cep);
      Lista_t.insert(controlador->saida, saida);

      // destruir_elemento(elemento);

      it = Lista_t.get_next(elementos[h], it);
    }
    
    Lista_t.destruir(elementos[h], destruir_elemento);
  }

}

int comando_dlezin(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;

  Ponto2D pos, size;

  pos  = Ponto2D_t.new(strtod(params[1], NULL), strtod(params[2], NULL));
  size = Ponto2D_t.new(strtod(params[3], NULL), strtod(params[4], NULL));

  Figura rect = cria_retangulo(pos.x, pos.y, size.x, size.y, "transparent", "black");
  set_opacity_figura(rect, 0.8);
  set_dashed_figura(rect, FIG_BORDA_TRACEJADA);
  
  Lista_t.insert(controlador->saida_svg_qry,
    cria_desenhavel(rect, get_svg_figura, destruir_figura));

  Ponto2D new_max = Ponto2D_t.add(pos, size);
  new_max         = Ponto2D_t.add_scalar(new_max, 4);

  controlador->max_qry = Ponto2D_t.maximo(controlador->max_qry, new_max);

  size = Ponto2D_t.add(pos, size);

  Lista *elementos = comando_d_all(this, controlador, pos, size, params[0]);
  
  remover_elementos(this, controlador, elementos);
  
  free(elementos);
  
  return 1;
}

int comando_dlezao(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;

  Ponto2D pos = Ponto2D_t.new(strtod(params[1], NULL), strtod(params[2], NULL));
  double r    = strtod(params[3], NULL);

  Figura circ = cria_circulo(pos.x, pos.y, r, "transparent", "black");
  set_opacity_figura(circ, 0.8);
  set_dashed_figura(circ, FIG_BORDA_TRACEJADA);

  Lista_t.insert(controlador->saida_svg_qry,
    cria_desenhavel(circ, get_svg_figura, destruir_figura));
    
  Ponto2D pA = Ponto2D_t.sub_scalar(pos, r);
  Ponto2D pB = Ponto2D_t.add_scalar(pos, r);

  Ponto2D new_max = Ponto2D_t.add_scalar(pos, r + 4);

  controlador->max_qry = Ponto2D_t.maximo(controlador->max_qry, new_max);

  Lista *elementos = comando_d_all(this, controlador, pA, pB, params[0]);
  
  // Como eh no circulo, tem que remover os que nao estao no circulo
  for (int h = 1; h < 4; h++) {
    if (!elementos[h]) continue;
    
    Posic it = Lista_t.get_first(elementos[h]);
    while (it) {
      Posic next_it = Lista_t.get_next(elementos[h], it);
      Ponto2D posicao_elemento = get_pos(Lista_t.get(elementos[h], it));
      
      // Se a figura estiver fora do circulo remover da lista
      if (Ponto2D_t.dist_squared(pos, posicao_elemento) > sqr(r)) {
        Lista_t.remove(elementos[h], it);
      }
      
      it = next_it;
    }

  }
  
  remover_elementos(this, controlador, elementos);
  
  free(elementos);
  
  return 1;
}

int comando_dzin(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;

  Ponto2D pos, size;
  
  pos  = Ponto2D_t.new(strtod(params[0], NULL), strtod(params[1], NULL));
  size = Ponto2D_t.new(strtod(params[2], NULL), strtod(params[3], NULL));

  Figura figura = cria_retangulo(pos.x, pos.y, size.x, size.y, "transparent", "black");
  set_opacity_figura(figura, 0.8);
  set_dashed_figura(figura, FIG_BORDA_TRACEJADA);

  Lista_t.insert(controlador->saida_svg_qry,
    cria_desenhavel(figura, get_svg_figura, destruir_figura));

  Ponto2D new_max = Ponto2D_t.add(pos, size);
  new_max         = Ponto2D_t.add_scalar(new_max, 4);

  controlador->max_qry = Ponto2D_t.maximo(controlador->max_qry, new_max);

  size = Ponto2D_t.add(pos, size);

  Lista *elementos = comando_d_all(this, controlador, pos, size, "q");

  // Checar se a quadra está inteiramente dentro da figura
  Lista quadras = elementos[0];
  Posic it      = (quadras) ? Lista_t.get_first(quadras) : NULL;
  while (it) {
    Posic next_it   = Lista_t.get_next(quadras, it);
    Elemento quadra = Lista_t.get(quadras, it);

    Figura figura_quadra = get_figura_elemento(quadra);

    if (!dentro_figura(figura, figura_quadra))
      Lista_t.remove(quadras, it);

    destruir_figura(figura_quadra);

    it = next_it;
  }

  remover_elementos(this, controlador, elementos);

  free(elementos);

  return 1;
}

int comando_dzao(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;

  Ponto2D pos;
  double r;
  
  r   = strtod(params[0], NULL);
  pos = Ponto2D_t.new(strtod(params[1], NULL), strtod(params[2], NULL));

  Figura figura = cria_circulo(pos.x, pos.y, r, "transparent", "black");
  set_opacity_figura(figura, 0.8);
  set_dashed_figura(figura, FIG_BORDA_TRACEJADA);

  Lista_t.insert(controlador->saida_svg_qry,
    cria_desenhavel(figura, get_svg_figura, destruir_figura));

  Ponto2D new_max = Ponto2D_t.add_scalar(pos, r + 4);

  controlador->max_qry = Ponto2D_t.maximo(controlador->max_qry, new_max);

  Ponto2D pA = Ponto2D_t.sub_scalar(pos, r);
  Ponto2D pB = Ponto2D_t.add_scalar(pos, r);

  Lista *elementos = comando_d_all(this, controlador, pA, pB, "q");

  // Checar se a quadra está inteiramente dentro da figura
  Lista quadras = elementos[0];
  Posic it      = (quadras) ? Lista_t.get_first(quadras) : NULL;
  while (it) {
    Posic next_it   = Lista_t.get_next(quadras, it);
    Elemento quadra = Lista_t.get(quadras, it);

    Figura figura_quadra = get_figura_elemento(quadra);

    if (!dentro_figura(figura, figura_quadra))
      Lista_t.remove(quadras, it);

    destruir_figura(figura_quadra);

    it = next_it;
  }

  remover_elementos(this, controlador, elementos);

  free(elementos);

  return 1;
}