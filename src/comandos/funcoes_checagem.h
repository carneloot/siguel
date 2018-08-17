#ifndef __FUNCOES_CHECAGEM_H__
#define __FUNCOES_CHECAGEM_H__

#include <modules/lista.h>
#include <stdarg.h>

int checar_id_figura(const Item _figura, const void *_id);

int elemento_dentro_figura(const Item _elemento, const void *_figura);

void compararCEP(const Item _ele, unsigned prof, va_list _list);

#endif /* __FUNCOES_CHECAGEM_H__ */