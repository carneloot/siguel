#include <stdio.h>

#include "arquivo.h"
#include "comando.h"
#include "controlador.h"
#include "lista.h"

int main(int argc, const char *argv[]) {
  Controlador controlador;
  Comando comando;
  Lista linhas;
  Node linha_atual_node;
  char *linha_atual;
  int eh_erro;

  /* Setup */
  controlador = cria_controlador();
  lidar_parametros(controlador, argc, argv);
  linhas = get_linhas_entrada(controlador);

  linha_atual_node = get_start_lista(linhas);

  eh_erro = !linhas;

  /* Main loop */
  while (tem_proximo_node(linha_atual_node) && !eh_erro) {
    linha_atual = (char *) get_proximo_node(&linha_atual_node);

    #ifdef DEBUG
    printf("DEBUG: %s\n", linha_atual);
    #endif

    comando = cria_comando(linha_atual);
    eh_erro = !executar_comando(controlador, comando);

    destruir_comando(comando);
  }

  if (eh_erro)
    printf(
      "Arquivo \"%s.geo\" finalizado com erro.\n", get_nome_base(controlador));
  else
    printf("Arquivo \"%s.geo\" finalizado.\n", get_nome_base(controlador));

  destruir_lista(linhas);

  destruir_controlador(controlador);

  return 0;
}