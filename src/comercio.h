#ifndef __COMERCIO_H__
#define __COMERCIO_H__

#include <endereco.h>

typedef void *Comercio;

Comercio cria_comercio(char *cnpj, char *tipo, char *cep, int face, int num, char *nome);

void comercio_destruir(Comercio this);

char *comercio_get_cnpj(Comercio this);

char *comercio_get_tipo(Comercio this);

char *comercio_get_info(Comercio _this, char *tipo_desc);

Endereco comercio_get_endereco(Comercio this);

#endif /* __COMERCIO_H__ */