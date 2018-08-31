#ifndef __COMERCIO_H__
#define __COMERCIO_H__

typedef void *Comercio;

Comercio cria_comercio(char *cnpj, char *tipo, char *cep, int face, int num, char *nome);

void comercio_destruir(Comercio this);

#endif /* __COMERCIO_H__ */