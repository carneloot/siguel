#include "utils.h"

#include <math.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

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

  saida = (char *) calloc(tam + 1, sizeof(char));

  /* Copia os 'tam' primeiros caracteres de entrada para saida */
  /* strncpy deu problema no valgrind (????) */
  for (i = 0; i < tam; i++)
    saida[i] = entrada[i];
  saida[i] = 0;

  return saida;
}

char *get_nome(char *path) {
  char *filename;
  int i, tam, count, start;

  tam   = strlen(path);
  i     = tam - 1;
  count = 0;

  // Vai de tras para frente até achar a primeira barra
  while (i >= 0 && path[i] != '/') {
    // Se achar o ponto, quer dizer que começa a contagem do tamanho do arquivo
    if (path[i] == '.')
      count = -1;
    count++;
    i--;
  }

  // Start indica o incio do nome do arquivo
  start = i + 1;

  // Transfere o nome para filename
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

  while (path[--i] != '/' && i > 0) {}

  count = i;

  diretorio = (char *) malloc((count + 2) * sizeof(char));
  for (i = 0; i < count; i++)
    diretorio[i] = path[i];
  diretorio[i] = 0;

  if (count != 0)
    strcat(diretorio, "/");

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
    tam -= 1;

  newDir = (char *) calloc(tam + 2, sizeof(char));

  strcat(newDir, dir);
  if (dir[tam] != '/')
    strcat(newDir, "/");

  return newDir;
}

char *remover_extensao(const char *path) {
  size_t length;
  char *saida;
  int j;

  int tamanho_extensao = 0;

  char *iterador = (char *) path + strlen(path) - 1;

  while (*iterador-- != '.')
    tamanho_extensao++;

  length = strlen(path) - 1 - tamanho_extensao;
  saida  = (char *) malloc((length + 1) * sizeof(char));
  for (j = 0; j < length; j++)
    saida[j] = path[j];
  saida[j] = 0;

  return saida;
}

char *format_string(const char *fmt, ...) {
  va_list list;
  va_start(list, fmt);
  size_t length = vsnprintf(NULL, 0, fmt, list);
  va_end(list);

  char *saida = malloc(length + 2);
  va_start(list, fmt);
  vsprintf(saida, fmt, list);
  va_end(list);

  return saida;
}