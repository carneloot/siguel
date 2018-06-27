#ifndef __CONTROLADOR_H__
#define __CONTROLADOR_H__

#include "arquivo.h"
#include "comando.h"

typedef void *Controlador;

Controlador cria_controlador();

void lidar_parametros(Controlador c, int argc, const char *argv[]);

int executar_comando(Controlador c);

int ha_comandos(Controlador c);

void gerar_fila_execucao(Controlador c);

void finalizar_arquivos(Controlador c);

void destruir_controlador(Controlador c);

char *get_nome_base(Controlador c);

#endif /* __CONTROLADOR_H__ */