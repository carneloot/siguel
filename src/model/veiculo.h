#ifndef __CARRO_H__
#define __CARRO_H__


#include <stdbool.h>

#include "modules/ponto2d.h"

typedef void* Veiculo;

Veiculo cria_veiculo( Ponto2D posicao, double w, double h, char *placa );

bool colide_veiculo( Veiculo v1, Veiculo v2 );




#endif /* __CARRO_H__ */