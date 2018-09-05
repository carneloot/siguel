#ifndef __CONTROLADOR_H__
#define __CONTROLADOR_H__

#include "arquivo.h"
#include "comando.h"

typedef void *Controlador;

Controlador cria_controlador();

void lidar_parametros(Controlador c, int argc, const char *argv[]);

int executar_proximo_comando(Controlador c);

int ha_comandos(Controlador c);

void gerar_fila_execucao(Controlador c);

void finalizar_arquivos(Controlador c);

void printar_mensagem_final(Controlador this, int eh_erro);

void destruir_controlador(Controlador c);

#endif /* __CONTROLADOR_H__ */