#include "funcoes_checagem.h"

#include <string.h>

#include <model/figura.h>
#include <model/elemento.h>
#include <model/utils.h>

int checar_id_figura(const Item _figura, const void *_id) {
  const Figura figura = (const Figura) _figura;
  const int id        = (const int) *(int *) _id;

  return !(get_id_figura(figura) == id);
}

void compararCEP(const Item _ele, unsigned prof, va_list _list) {
  va_list list;
  va_copy(list, _list);

  // CEP, *RESULT
  char *cep          = va_arg(list, char *);
  Elemento *result   = va_arg(list, Elemento *);
  const Elemento ele = _ele;

  if (*result)
    return;

  if (!strcmp(get_id_elemento(ele), cep))
    *result = ele;
}

int elemento_dentro_rect(void *_elemento, int dim, void *_pA, void *_pB) {
  Elemento elemento = (Elemento) _elemento;
  
  Ponto2D *pA = _pA;
  Ponto2D *pB = _pB;
  
  Ponto2D min = Ponto2D_t.new(
    min(pA->x, pB->x),
    min(pA->y, pB->y)
  );

  Ponto2D max = Ponto2D_t.new(
    max(pA->x, pB->x),
    max(pA->y, pB->y)
  ); 
  
  Ponto2D pos = get_pos(elemento);
  
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
