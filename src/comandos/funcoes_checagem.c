#include "funcoes_checagem.h"

#include <string.h>

#include <figura.h>
#include <elemento.h>
#include <utils.h>

int checar_id_figura(void *_figura, void *_id) {
  Figura figura = (Figura) _figura;
  int id        = (int) *(int *) _id;

  return !(get_id_figura(figura) == id);
}

void compararCEP(void *_ele, unsigned prof, va_list list) {
  // CEP, *RESULT
  char *cep          = va_arg(list, char *);
  Elemento *result   = va_arg(list, Elemento *);
  Elemento ele = _ele;

  if (*result)
    return;

  if (!strcmp(get_id_elemento(ele), cep))
    *result = ele;
}

int elemento_dentro_rect(void *_elemento, int dim, void *_pA, void *_pB) {
  Elemento elemento = (Elemento) _elemento;
  
  Ponto2D_t *pA = _pA;
  Ponto2D_t *pB = _pB;
  
  Ponto2D_t min = p2d_new(
    min(pA->x, pB->x),
    min(pA->y, pB->y)
  );

  Ponto2D_t max = p2d_new(
    max(pA->x, pB->x),
    max(pA->y, pB->y)
  ); 
  
  Ponto2D_t pos = get_pos(elemento);
  
  switch (dim) {
    case -1:
      return !(
        pos.x < min.x || pos.x > max.x ||
        pos.y < min.y || pos.y > max.y
      );
      
    case 0: 
      if (pos.x < min.x)      return -1;
      else if (pos.x > max.x) return  1;
      return 0;
      
    case 1: 
      if (pos.y < min.y)     return -1;
      else if(pos.y > max.y) return  1;
      return 0;
  }
  
  return 0;
}
