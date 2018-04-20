#include "comando.h"

#include <stdlib.h>
#include <string.h>

struct Comando {
  enum TipoComando tipo;
  int num_param;
  char **params;
};

// Comandos definidos numa constante para procurar automaticamente
const struct {
  char *id;
  enum TipoComando tipo;
} comandos[] = {{"nx", MUDAR_NUM_FIGURAS}, {"c", DESENHA_CIRCULO},
                {"r", DESENHA_RETANGULO},  {"o", CHECA_SOBREPOSICAO},
                {"i", CHECA_PONTO},        {"d", DISTANCIA_FIGURAS},
                {"a", CRIAR_SVG},          {"#", FIM_DO_ARQUIVO}};

Comando cria_comando(char *entrada) {
  int h;
  char *linha, *item;

  struct Comando *this = (struct Comando *) malloc(sizeof(struct Comando));

  this->tipo      = NONE;
  this->num_param = 0;

  linha = (char *) malloc((strlen(entrada) + 1) * sizeof(char));
  strcpy(linha, entrada);

  // Conta o numero de parametros tem no comando
  item = strtok(linha, " ");
  while (item) {
    this->num_param++;
    item = strtok(NULL, " ");
  }
  this->num_param--;

  // Coloca os parametros no vetor de strings this->params
  this->params = (char **) malloc(this->num_param * sizeof(char *));
  strcpy(linha, entrada);

  h = 0;

  item = strtok(linha, " ");
  while (item) {
    // Tipo
    if (this->tipo == NONE) {
      int total_comandos = sizeof(comandos) / sizeof(comandos[0]);

      for (int i = 0; i < total_comandos; i++) {
        if (!strcmp(item, comandos[i].id)) {
          this->tipo = comandos[i].tipo;
          break;
        }
      }
    }

    // Parametros
    else {
      this->params[h] = (char *) malloc((strlen(item) + 1) * sizeof(char));
      strcpy(this->params[h], item);

      h++;
    }

    item = strtok(NULL, " ");
  }

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