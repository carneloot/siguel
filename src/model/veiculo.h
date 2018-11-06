#ifndef __CARRO_H__
#define __CARRO_H__


#include <stdbool.h>
#include "modules/ponto2d.h"

typedef void* Veiculo;

Veiculo cria_veiculo( double x, double y, double w, double h, char *placa );

bool colide_veiculo( Veiculo this, Veiculo other );

void destruir_veiculo( Veiculo this );

char* get_placa_veiculo( Veiculo this );




#endif /* __CARRO_H__ */