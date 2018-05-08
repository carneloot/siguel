#include "utils.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

float fmaxf(float a, float b) {
  return (a > b) ? a : b;
}

float fminf(float a, float b) {
  return (a < b) ? a : b;
}

char *trim(char *entrada) {
  int tam, i;
  char *saida;

  tam = strlen(entrada);

  i = tam - 1;
  while ((entrada[i] == '\n' || entrada[i] == '\r' || entrada[i] == '\t' ||
          entrada[i] == ' ') &&
         i >= 0) {
    i--;
    tam--;
  }

  saida = (char *) malloc((tam + 1) * sizeof(char));

  /* Copia os 'tam' primeiros caracteres de entrada para saida */
  /* strncpy deu problema no valgrind (????) */
  for (i = 0; i < tam; i++)
    saida[i] = entrada[i];
  saida[i] = 0;

  free(entrada);
  return saida;
}

int num_digits(float num) {
  int a, count;

  if (num == 0)
    return 3;

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

char *evaluate_dir(char *dir) {
  int tam;
  char *newDir;

  tam = strlen(dir);

  if (dir[tam - 1] == '/')
    return dir;

  newDir = (char *) calloc(tam + 2, sizeof(char));

  strcat(newDir, dir);
  strcat(newDir, "/");

  if (dir)
    free(dir);

  return newDir;
}