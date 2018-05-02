#include "utils.h"

#include <stdlib.h>
#include <string.h>

float max(float a, float b) {
  return (a > b) ? a : b;
}

float min(float a, float b) {
  return (a < b) ? a : b;
}

void trim(char **p) {
  int tam, i;
  char *entrada, *saida;

  entrada = *p;
  tam     = strlen(entrada);

  i = tam - 1;
  while ((entrada[i] == '\n' || entrada[i] == '\r' || entrada[i] == '\t' ||
          entrada[i] == ' ') &&
         i >= 0) {
    i--;
    tam--;
  }

  saida = (char *) malloc((tam + 1) * sizeof(char));

  // Copia os 'tam' primeiros caracteres de entrada para saida
  // strncpy deu problema no valgrind (????)
  for (i = 0; i < tam; i++)
    saida[i] = entrada[i];
  saida[i] = 0;

  free(entrada);
  *p = saida;
}

int num_digits(float num) {
  if (num == 0)
    return 3;

  int a, count;

  a     = (int) num;
  count = 0;

  while (a >= 10) {
    a = a / 10;
    count++;
  }
  count++;

  return count + 2;
}