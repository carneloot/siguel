#ifndef __FUNCOES_CHECAGEM_H__
#define __FUNCOES_CHECAGEM_H__

#include <modules/lista.h>
#include <stdarg.h>

int checar_id_figura(const Item _figura, const void *_id);

void compararCEP(const Item _ele, unsigned prof, va_list _list);

int elemento_dentro_rect(void *_elemento, int dim, void *_pA, void *_pB);

#endif /* __FUNCOES_CHECAGEM_H__ */