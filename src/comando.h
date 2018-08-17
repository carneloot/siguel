#ifndef __COMANDO_H__
#define __COMANDO_H__

#include "comando.r"

typedef struct Comando *Comando;

Comando cria_comando(char *entrada);

void destruir_comando(Comando c);

#endif /* __COMANDO_H__ */