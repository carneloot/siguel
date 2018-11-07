#ifndef __CARRO_H__
#define __CARRO_H__


#include <stdbool.h>
#include <model/modules/ponto2d.h>
#include <model/figura.h>

typedef void* Veiculo;

/**
 * Cria um veículo e retorna um ponteiro para ele.
 * Sobre char placa: A função faz uma cópia de placa
 *  logo, não é necessário alocar memória dinamicamente para ela.
 */
Veiculo cria_veiculo( Ponto2D pos, double w, double h, char *placa );

bool colide_veiculo( Veiculo this, Veiculo other );

void destruir_veiculo( Veiculo this );

bool igual_veiculo(Veiculo this, Veiculo other);

char* get_placa_veiculo( Veiculo this );

Figura get_figura_veiculo( Veiculo this );


#endif /* __CARRO_H__ */