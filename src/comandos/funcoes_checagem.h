#ifndef __FUNCOES_CHECAGEM_H__
#define __FUNCOES_CHECAGEM_H__

#include <modules/lista.h>
#include <stdarg.h>

int checar_id_figura(void *figura, void *id);

void compararCEP(void *ele, unsigned prof, va_list list);

int elemento_dentro_rect(void *elemento, int dim, void *pA, void *pB);

#endif /* __FUNCOES_CHECAGEM_H__ */