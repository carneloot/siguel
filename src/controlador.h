#ifndef __CONTROLADOR_H__
#define __CONTROLADOR_H__

#include <comando.h>

#include <model/estrutural/arquivo.h>

#include <model/colecao/hash.h>

typedef void *Controlador;

Controlador cria_controlador();

void lidar_parametros(Controlador c, int argc, const char *argv[]);

int executar_proximo_comando(Controlador c);

int ha_comandos(Controlador c);

void gerar_fila_execucao(Controlador c);

void finalizar_arquivos(Controlador c);

void printar_mensagem_final(Controlador this, int eh_erro);

void destruir_controlador(Controlador c);

HashTable_t get_table_quadras(Controlador this);

#endif /* __CONTROLADOR_H__ */