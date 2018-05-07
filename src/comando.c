#include "comando.h"

#include <stdlib.h>
#include <string.h>

struct Comando {
  enum TipoComando tipo;
  int num_param;
  char **params;
};

/* Comandos definidos numa constante para procurar automaticamente */
const struct {
  char *id;
  enum TipoComando tipo;
} comandos[] = {{"nx", MUDAR_NUM_FIGURAS},
                {"c", DESENHA_CIRCULO},
                {"r", DESENHA_RETANGULO},
                {"o", CHECA_SOBREPOSICAO},
                {"i", CHECA_PONTO},
                {"d", DISTANCIA_FIGURAS},
                {"a", CRIAR_SVG},
                {"#", FIM_DO_ARQUIVO}};

int conta_params(char *entrada);

Comando cria_comando(char *entrada) {
  int h, i;
  char *linha, *item;

  struct Comando *this;
  this = (struct Comando *) malloc(sizeof(struct Comando));

  this->tipo      = NONE;
  this->num_param = conta_params(entrada);

  linha = (char *) malloc((strlen(entrada) + 1) * sizeof(char));

  /* Coloca os parametros no vetor de strings this->params */
  this->params = (char **) malloc(this->num_param * sizeof(char *));
  strcpy(linha, entrada);

  h = 0;

  item = strtok(linha, " ");
  while (item) {
    /* Tipo */
    if (this->tipo == NONE) {
      int total_comandos = sizeof(comandos) / sizeof(comandos[0]);

      for (i = 0; i < total_comandos; i++) {
        if (!strcmp(item, comandos[i].id)) {
          this->tipo = comandos[i].tipo;
          break;
        }
      }
    }

    /* Parametros */
    else {
      this->params[h] = (char *) malloc((strlen(item) + 1) * sizeof(char));
      strcpy(this->params[h], item);

      h++;
    }

    item = strtok(NULL, " ");
  }

  free(linha);

  return (void *) this;
}

int get_numero_parametros(Comando c) {
  return ((struct Comando *) c)->num_param;
}

char **get_parametros(Comando c) {
  return ((struct Comando *) c)->params;
}

enum TipoComando get_tipo(Comando c) {
  return ((struct Comando *) c)->tipo;
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