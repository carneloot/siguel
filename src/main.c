#include <stdio.h>

#include "controlador.h"
#include "modules/logger.h"

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
    eh_erro = !executar_proximo_comando(controlador);
  }
  
  LOG_PRINT(LOG_FILE, "Todos comandos executados. Finalizando arquivos");

  finalizar_arquivos(controlador);

  printar_mensagem_final(controlador, eh_erro);

  destruir_controlador(controlador);

  return 0;
}