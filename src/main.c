#include <stdio.h>

#include "controlador.h"
#include "modules/logger.h"

int main(int argc, const char *argv[]) {
  Controlador controlador;
  char *nome_base;
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

  nome_base = get_nome_base(controlador);

  finalizar_arquivos(controlador);

  if (eh_erro) {
    LOG_PRINT(LOG_STDOUT, "Arquivo  \"%s.geo\" finalizado com erro.", nome_base);
  } else {
    char *nome_query = get_nome_query(controlador);
    if (nome_query)
      LOG_PRINT(
        LOG_STDOUT,
        "Arquivos \"%s.geo\" e \"%s.qry\" finalizados.",
        nome_base,
        nome_query);
    else
      LOG_PRINT(LOG_STDOUT, "Arquivo  \"%s.geo\" finalizado.", nome_base);
  }

  destruir_controlador(controlador);

  return 0;
}