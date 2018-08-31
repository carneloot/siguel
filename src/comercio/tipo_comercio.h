#ifndef __TIPO_COMERCIO_H__
#define __TIPO_COMERCIO_H__

typedef void *TipoComercio;

TipoComercio cria_tipo_comercio(char *identificacao, char *descricao);

void tipo_comercio_destruir(TipoComercio this);

#endif /* __TIPO_COMERCIO_H__ */