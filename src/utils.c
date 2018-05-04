#include "utils.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

float round_digits(float num, int n) {
  float newNum;

  newNum = num * pow(10, n);
  newNum = (float) round(newNum);
  newNum = newNum / pow(10, n);

  return newNum;
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

float dist_squared(float x1, float y1, float x2, float y2) {
  return pow(x1 - x2, 2) + pow(y1 - y2, 2);
}

float dist(float x1, float y1, float x2, float y2) {
  return sqrt(dist_squared(x1, y1, x2, y2));
}

char *get_nome(char *path) {
  char *filename;
  int i, tam, count, start;

  tam   = strlen(path);
  i     = tam;
  count = 0;

  while (path[--i] != '/') {
    if (path[i] == '.')
      count = -1;
    count++;
  }
  start = i + 1;

  filename = (char *) malloc((count + 1) * sizeof(char));
  for (i = 0; i < count; i++)
    filename[i] = path[start + i];
  filename[i] = 0;

  return filename;
}

char *get_diretorio(char *path) {
  char *diretorio;
  int tam, i, count;

  tam = strlen(path);
  i   = tam;

  while (path[--i] != '/') {}

  count = i;

  diretorio = (char *) malloc((count + 1) * sizeof(char));
  for (i = 0; i < count; i++)
    diretorio[i] = path[i];
  diretorio[i] = 0;

  return diretorio;
}

char *get_extensao(char *path) {
  int count, tam, i;
  char *ext;

  count = 0;
  tam   = strlen(path);

  for (i = tam - 1; i >= 0; i--) {
    if (path[i] == '.')
      break;
    count++;
  }

  ext = (char *) malloc((count + 1) * sizeof(char));

  for (i = 0; i < count; i++)
    ext[i] = path[tam - count + i];
  ext[i] = 0;

  return ext;
}