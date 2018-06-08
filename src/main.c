#include <stdio.h>

#include "arquivo.h"
#include "controlador.h"

int main(int argc, const char *argv[]) {
  Controlador controlador;
  int eh_erro;

  /* Setup */
  controlador = cria_controlador();
  lidar_parametros(controlador, argc, argv);
  gerar_fila_execucao(controlador);

  eh_erro = 0;

  /* Main loop */
  while (ha_comandos(controlador) && !eh_erro) {
    eh_erro = !executar_comando(controlador);
  }

  if (eh_erro)
    printf(
      "Arquivo \"%s.geo\" finalizado com erro.\n", get_nome_base(controlador));
  else
    printf("Arquivo \"%s.geo\" finalizado.\n", get_nome_base(controlador));

  destruir_controlador(controlador);

  return 0;
}