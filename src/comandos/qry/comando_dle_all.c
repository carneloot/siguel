#include <comando.r>
#include <controlador.r>

#include <elemento.h>
#include <figura.h>
#include <stdlib.h>
#include <string.h>
#include <utils.h>
#include "../funcoes_checagem.h"
#include <modules/logger.h>

// TODO: Implementar para KDTree
static Lista *__comando_dle_all(
  struct Comando *this,
  struct Controlador *controlador,
  Ponto2D pA, Ponto2D pB,
  const char *tipos) {
  int tipos_pesquisa[4] = {0, 0, 0, 0};

  for (int i = 0; i < strlen(tipos); i++) {
    switch (tipos[i]) {
      case 'h': tipos_pesquisa[HIDRANTE]   = 1; break;
      case 's': tipos_pesquisa[SEMAFORO]   = 1; break;
      case 'r': tipos_pesquisa[RADIO_BASE] = 1; break;
    }
  }
  
  Lista *saida = calloc(4, sizeof(*saida));
  
  // Procurar o elemento dentro da figura colocar no vetor de listas
  for (int i = 1; i < 4; i++) {
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
  struct Controlador *controlador,
  Lista *elementos) {
  
  int h = 1;
  while (elementos[h]) {
    if (!elementos[h]) { h++; continue; }
    
    Posic it = Lista_t.get_first(elementos[h]);
    while (it) {
      Elemento ele = Lista_t.get(elementos[h], it);
      
      KDTree_t.delete(controlador->elementos[h], ele);
      
      it = Lista_t.get_next(elementos[h], it);
    }
    
    Lista_t.destruir(elementos[h], NULL);
    
    h++;
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

  Lista *elementos = __comando_dle_all(this, controlador, pos, size, params[0]);
  
  remover_elementos(controlador, elementos);
  
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

  Lista *elementos = __comando_dle_all(this, controlador, pA, pB, params[0]);
  
  // Como eh no circulo, tem que remover os que nao estao no circulo
  /*int h = 1;
  while (elementos[h]) {
    if (!elementos[h]) { h++; continue; }
    
    Posic it = Lista_t.get_first(elementos[h]);
    while (it) {
      Posic next_it = Lista_t.get_next(elementos[h], it);
      Ponto2D posicao_elemento = get_pos(Lista_t.get(elementos[h], it));
      
      // Se a figura estiver fora do circulo remover da lista
      if (Ponto2D_t.dist_squared(pos, posicao_elemento) > r * r) {
        Lista_t.remove(elementos[h], it);
      }
      
      it = next_it;
    }
    
    h++;
  }*/
  
  remover_elementos(controlador, elementos);
  
  free(elementos);
  
  return 1;
}