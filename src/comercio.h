#ifndef __COMERCIO_H__
#define __COMERCIO_H__

typedef void *Comercio;

Comercio cria_comercio(char *tipo, char *descricao);

void comercio_destruir(Comercio this);

#endif /* __COMERCIO_H__ */