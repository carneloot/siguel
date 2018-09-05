#include <comando.r>
#include <controlador.r>

#include <elemento.h>
#include <figura.h>
#include <stdlib.h>
#include <string.h>
#include <utils.h>
#include "../funcoes_checagem.h"
#include <modules/logger.h>

static Lista *__comando_d_all(
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
  
  for (int h = 0; h < 4; h++) {
    if (!elementos[h]) continue;

    const char *tipo_elemento = get_tipo_string_elemento(h);

    Posic it = Lista_t.get_first(elementos[h]);
    while (it) {
      Elemento elemento = Lista_t.get(elementos[h], it);

      KDTree_t.delete(controlador->elementos[h], elemento);
      char *cep   = get_cep_elemento(elemento);
      char *saida = malloc(18 + strlen(tipo_elemento) + strlen(cep));
      sprintf(saida, "%s: %s deletado (a).\n", tipo_elemento, cep);
      Lista_t.insert(controlador->saida, saida);

      // FIXME: Quando o elemento é deletado, ele nao funciona para equs.qry e para quads.qrys
      // destruir_elemento(elemento);

      it = Lista_t.get_next(elementos[h], it);
    }
    
    Lista_t.destruir(elementos[h], NULL);
  }

}

int __comando_dlezin(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;

  Ponto2D pos, size;

  pos  = Ponto2D_t.new(strtod(params[1], NULL), strtod(params[2], NULL));
  size = Ponto2D_t.new(strtod(params[3], NULL), strtod(params[4], NULL));
  
  Lista_t.insert(controlador->saida_svg_qry,
    cria_retangulo(pos.x, pos.y, size.x, size.y, "transparent", "black"));

  Ponto2D new_max = Ponto2D_t.add(pos, size);
  new_max         = Ponto2D_t.add_scalar(new_max, 4);

  controlador->max_qry.x = max(controlador->max_qry.x, new_max.x);
  controlador->max_qry.y = max(controlador->max_qry.y, new_max.y);

  size = Ponto2D_t.add(pos, size);

  Lista *elementos = __comando_d_all(this, controlador, pos, size, params[0]);
  
  remover_elementos(this, controlador, elementos);
  
  free(elementos);
  
  return 1;
}

int __comando_dlezao(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;

  Ponto2D pos = Ponto2D_t.new(strtod(params[1], NULL), strtod(params[2], NULL));
  double r    = strtod(params[3], NULL);

  Lista_t.insert(controlador->saida_svg_qry,
    cria_circulo(pos.x, pos.y, r, "transparent", "black"));
    
  Ponto2D pA = Ponto2D_t.sub_scalar(pos, r);
  Ponto2D pB = Ponto2D_t.add_scalar(pos, r);

  Ponto2D new_max = Ponto2D_t.add_scalar(pos, r + 4);

  controlador->max_qry.x = max(controlador->max_qry.x, new_max.x);
  controlador->max_qry.y = max(controlador->max_qry.y, new_max.y);

  Lista *elementos = __comando_d_all(this, controlador, pA, pB, params[0]);
  
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

int __comando_dzin(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;

  Ponto2D pos, size;
  
  pos  = Ponto2D_t.new(strtod(params[0], NULL), strtod(params[1], NULL));
  size = Ponto2D_t.new(strtod(params[2], NULL), strtod(params[3], NULL));

  Figura figura = cria_retangulo(pos.x, pos.y, size.x, size.y, "transparent", "black");

  Lista_t.insert(controlador->saida_svg_qry, figura);

  Ponto2D new_max = Ponto2D_t.add(pos, size);
  new_max         = Ponto2D_t.add_scalar(new_max, 4);

  controlador->max_qry.x = max(controlador->max_qry.x, new_max.x);
  controlador->max_qry.y = max(controlador->max_qry.y, new_max.y);

  size = Ponto2D_t.add(pos, size);

  Lista *elementos = __comando_d_all(this, controlador, pos, size, "q");

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

int __comando_dzao(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;

  Ponto2D pos;
  double r;
  
  r   = strtod(params[0], NULL);
  pos = Ponto2D_t.new(strtod(params[1], NULL), strtod(params[2], NULL));

  Figura figura = cria_circulo(pos.x, pos.y, r, "transparent", "black");

  Lista_t.insert(controlador->saida_svg_qry, figura);

  Ponto2D new_max = Ponto2D_t.add_scalar(pos, r + 4);

  controlador->max_qry.x = max(controlador->max_qry.x, new_max.x);
  controlador->max_qry.y = max(controlador->max_qry.y, new_max.y);

  Ponto2D pA = Ponto2D_t.sub_scalar(pos, r);
  Ponto2D pB = Ponto2D_t.add_scalar(pos, r);

  Lista *elementos = __comando_d_all(this, controlador, pA, pB, "q");

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