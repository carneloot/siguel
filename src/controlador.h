#ifndef _CONTROLADOR_H_
#define _CONTROLADOR_H_

#include "comando.h"
#include "arquivo.h"

typedef struct Controlador* Controlador;

Controlador cria_controlador();

void lidar_parametros(Controlador c, int argc, char* argv[]);

int executar_comando(Controlador c, Comando com);

void set_arquivo_entrada(Controlador c, Arquivo a);

#endif // _CONTROLADOR_H_