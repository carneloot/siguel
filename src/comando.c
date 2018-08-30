#include "comando.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "comando.r"

#define EXPAND_AS_DECLARATION(a, b) extern int __##a(void *this, void *controlador);
#define EXPAND_AS_JUMPTABLE(a, b) {b, __##a},
#define EXPAND_AS_SIZE_STRUCT(a, b) uint8_t a;
#define COMMAND_TABLE(ENTRY)    \
  ENTRY(    comando_nx,   "nx") \
  ENTRY(     comando_c,    "c") \
  ENTRY(     comando_r,    "r") \
  ENTRY(     comando_o,    "o") \
  ENTRY(     comando_i,    "i") \
  ENTRY(     comando_d,    "d") \
  ENTRY(     comando_a,    "a") \
  ENTRY(  comando_hash,    "#") \
  ENTRY(     comando_q,    "q") \
  ENTRY(     comando_h,    "h") \
  ENTRY(     comando_s,    "s") \
  ENTRY(     comando_t,    "t") \
  ENTRY(    comando_cq,   "cq") \
  ENTRY(    comando_ch,   "ch") \
  ENTRY(    comando_cs,   "cs") \
  ENTRY(    comando_ct,   "ct") \
  ENTRY(  comando_qzin,   "q?") \
  ENTRY(  comando_qzao,   "Q?") \
  ENTRY(  comando_dzin,   "dq") \
  ENTRY(  comando_dzao,   "Dq") \
  ENTRY(comando_dlezin,  "dle") \
  ENTRY(comando_dlezao,  "Dle") \
  ENTRY(    comando_cc,   "cc") \
  ENTRY(   comando_crd, "crd?") \
  ENTRY(   comando_crb, "crb?")

COMMAND_TABLE(EXPAND_AS_DECLARATION)

typedef struct {
  COMMAND_TABLE(EXPAND_AS_SIZE_STRUCT)
} size_struct_t;
#define NUM_COMANDOS sizeof(size_struct_t)

const struct {
  char *id;
  int (*executar)(void *this, void *controlador);
} comandos[] = {
  COMMAND_TABLE(EXPAND_AS_JUMPTABLE)
};

static int conta_params(char *entrada);

Comando cria_comando(char *entrada) {
  int h, i;
  char *linha, *item;

  struct Comando *this;
  this = (struct Comando *) malloc(sizeof(struct Comando));

  this->num_param = conta_params(entrada);

  linha = (char *) malloc((strlen(entrada) + 1) * sizeof(char));

  /* Coloca os parametros no vetor de strings this->params */
  this->params = (char **) malloc(this->num_param * sizeof(char *));
  strcpy(linha, entrada);

  h = 0;

  item = strtok(linha, " ");

  for (i = 0; i < NUM_COMANDOS; i++) {
    if (!strcmp(item, comandos[i].id)) {
      this->executar = comandos[i].executar;
      break;
    }
  }

  // Se nao achar o comando, quer dizer que ele nao faz nada, então retorna nulo
  if (i == NUM_COMANDOS) {
    free(linha);
    free(this->params);
    free(this);
    return NULL;
  }

  item = strtok(NULL, " ");

  while (item) {
    /* Parametros */
    this->params[h] = (char *) malloc((strlen(item) + 1) * sizeof(char));
    strcpy(this->params[h], item);
    h++;

    item = strtok(NULL, " ");
  }

  free(linha);

  return (void *) this;
}

void destruir_comando(Comando c) {
  struct Comando *this;
  int i;

  this = (struct Comando *) c;

  for (i = 0; i < this->num_param; i++) {
    free(this->params[i]);
  }

  free(this->params);

  free(c);
}

/* --------- FUNCOES PRIVADAS ------------- */

int conta_params(char *entrada) {
  char *linha, *item;
  int num_params = 0;

  linha = (char *) malloc((strlen(entrada) + 1) * sizeof(char));
  strcpy(linha, entrada);

  /* Conta o numero de parametros tem no comando */
  item = strtok(linha, " ");
  while (item) {
    num_params++;
    item = strtok(NULL, " ");
  }

  num_params--;

  free(linha);

  return num_params;
}