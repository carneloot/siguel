#include <stdio.h>
#include <stdlib.h>

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

  /* Setup */
  controlador = cria_controlador();
  lidar_parametros(controlador, argc, argv);
  linhas = get_linhas_entrada(controlador);

  linha_atual_node = get_start_lista(linhas);

  /* Main loop */
  while (tem_proximo_node(linha_atual_node)) {
    linha_atual = (char *) get_proximo_node(&linha_atual_node);

    comando = cria_comando(linha_atual);
    executar_comando(controlador, comando);

    destruir_comando(comando);
  }

  printf("Arquivo \"%s.geo\" finalizado.\n", get_nome_base(controlador));

  destruir_lista(linhas);

  destruir_controlador(controlador);

  return 0;
}