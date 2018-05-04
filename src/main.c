#include <stdio.h>
#include <stdlib.h>

#include "arquivo.h"
#include "comando.h"
#include "controlador.h"

int main(int argc, const char *argv[]) {
  Arquivo entrada;
  Controlador controlador;
  Comando comando;
  char *linha_atual;

  controlador = cria_controlador();
  lidar_parametros(controlador, argc, argv);
  abrir_arquivos(controlador);
  entrada = get_arquivo_entrada(controlador);

  while ((linha_atual = ler_proxima_linha(entrada))) {
    comando = cria_comando(linha_atual);
    executar_comando(controlador, comando);

    destruir_comando(comando);
    free(linha_atual);
  }

  printf("Arquivo \"%s.geo\" finalizado.\n", get_nome_base(controlador));

  destruir_controlador(controlador);

  return 0;
}