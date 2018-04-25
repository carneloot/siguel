#include "controlador.h"
#include "figura.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOTAL_FIGURAS_DEFAULT 1000

struct Controlador {
  Arquivo arquivo_entrada;
  char *nome_arquivo_entrada;
  char *dir_saida;
  char *dir_entrada;

  Figura *figuras;
  int total_figuras;
  int max_figuras;
};

Controlador cria_controlador() {
  struct Controlador *this =
      (struct Controlador *) malloc(sizeof(struct Controlador));

  this->arquivo_entrada      = NULL;
  this->nome_arquivo_entrada = NULL;
  this->dir_saida            = NULL;
  this->dir_entrada          = (char *) malloc(3 * sizeof(char));
  strcpy(this->dir_entrada, "./");

  this->max_figuras   = TOTAL_FIGURAS_DEFAULT;
  this->total_figuras = 0;
  this->figuras       = (Figura *) malloc(this->total_figuras * sizeof(Figura));

  return (void *) this;
}

void lidar_parametros(Controlador c, int argc, char *argv[]);

int executar_comando(Controlador c, Comando com);

void abrir_arquivo_entrada(Controlador c) {
  struct Controlador *this = (struct Controlador *) c;

  // Nome nao foi setado ainda
  if (!this->nome_arquivo_entrada)
    return;

  size_t tamanho_total =
      strlen(this->dir_entrada) + strlen(this->nome_arquivo_entrada) + 1;

  char *full_path = (char *) malloc(tamanho_total * sizeof(char));

  sprintf(full_path, "%s%s", this->dir_entrada, this->nome_arquivo_entrada);

  Arquivo arq = abrir_arquivo(full_path, LEITURA);

  if (!arq)
    return;

  this->arquivo_entrada = arq;

  free(full_path);
}

void destruir_controlador(Controlador c) {
  struct Controlador *this = (struct Controlador *) c;

  if (this->arquivo_entrada)
    fechar_arquivo(this->arquivo_entrada);

  for (int i = 0; i < this->total_figuras; i++)
    destruir_figura(this->figuras[i]);
  free(this->figuras);

  if (this->nome_arquivo_entrada)
    free(this->nome_arquivo_entrada);
  if (this->dir_saida)
    free(this->dir_saida);
  if (this->dir_entrada)
    free(this->dir_entrada);

  free(c);
}