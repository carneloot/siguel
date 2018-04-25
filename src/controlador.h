#ifndef __CONTROLADOR_H__
#define __CONTROLADOR_H__

#include "arquivo.h"
#include "comando.h"

typedef void *Controlador;

Controlador cria_controlador();

void lidar_parametros(Controlador c, int argc, char *argv[]);

int executar_comando(Controlador c, Comando com);

void abrir_arquivo_entrada(Controlador c);

void destruir_controlador(Controlador c);

#endif  // __CONTROLADOR_H__