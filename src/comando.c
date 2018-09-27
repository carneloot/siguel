#include "comando.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <utils.h>

#include "comando.r"

#define EXPAND_AS_DECLARATION(a, b) extern int a(void *this, void *controlador);
#define EXPAND_AS_JUMPTABLE(a, b) {b, a},
#define EXPAND_AS_SIZE_STRUCT(a, b) uint8_t a;
#define COMMAND_TABLE(ENTRY)    \
  ENTRY(       comando_nx,    "geo/nx") \
  ENTRY(        comando_c,     "geo/c") \
  ENTRY(        comando_r,     "geo/r") \
  ENTRY(        comando_o,     "geo/o") \
  ENTRY(        comando_i,     "geo/i") \
  ENTRY(        comando_d,     "geo/d") \
  ENTRY(        comando_a,     "geo/a") \
  ENTRY(     comando_hash,     "geo/#") \
  ENTRY(        comando_q,     "geo/q") \
  ENTRY(        comando_h,     "geo/h") \
  ENTRY(        comando_s,     "geo/s") \
  ENTRY(        comando_t,     "geo/t") \
  ENTRY(       comando_cq,    "geo/cq") \
  ENTRY(       comando_ch,    "geo/ch") \
  ENTRY(       comando_cs,    "geo/cs") \
  ENTRY(       comando_ct,    "geo/ct") \
  ENTRY(     comando_qzin,    "qry/q?") \
  ENTRY(     comando_qzao,    "qry/Q?") \
  ENTRY(     comando_dzin,    "qry/dq") \
  ENTRY(     comando_dzao,    "qry/Dq") \
  ENTRY(   comando_dlezin,   "qry/dle") \
  ENTRY(   comando_dlezao,   "qry/Dle") \
  ENTRY(       comando_cc,    "qry/cc") \
  ENTRY(      comando_crd,  "qry/crd?") \
  ENTRY(      comando_crb,  "qry/crb?") \
  ENTRY(    comando_qry_m,    "qry/m?") \
  ENTRY(   comando_qry_mr,   "qry/mr?") \
  ENTRY(   comando_qry_dm,   "qry/dm?") \
  ENTRY(   comando_qry_de,   "qry/de?") \
  ENTRY(  comando_qry_rip,   "qry/rip") \
  ENTRY(  comando_qry_fec,   "qry/fec") \
  ENTRY(  comando_qry_mud,   "qry/mud") \
  ENTRY(comando_qry_mudec, "qry/mudec") \
  ENTRY(  comando_qry_ecr,  "qry/ecr?") \
  ENTRY( comando_qry_tecr, "qry/tecr?") \
  ENTRY(  comando_qry_ecq,  "qry/ecq?") \
  ENTRY( comando_qry_tecq, "qry/tecq?") \
  ENTRY(  comando_qry_hmp,  "qry/hmp?") \
  ENTRY( comando_qry_hmpe, "qry/hmpe?") \
  ENTRY(  comando_qry_dpr,   "qry/dpr") \
  ENTRY(     comando_ec_t,      "ec/t") \
  ENTRY(     comando_ec_e,      "ec/e") \
  ENTRY(     comando_pm_p,      "pm/p") \
  ENTRY(     comando_pm_m,      "pm/m")

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

Comando cria_comando(char *entrada, char *arq) {
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

  char *cod = format_string("%s/%s", arq, item);

  for (i = 0; i < NUM_COMANDOS; i++) {
    if (!strcmp(cod, comandos[i].id)) {
      this->executar = comandos[i].executar;
      break;
    }
  }

  free(cod);

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