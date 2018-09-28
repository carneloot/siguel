#include "arquivo.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <model/modules/logger.h>
#include <model/utils.h>

struct Arquivo {
  FILE *arq;
  int linha_atual;
  enum TipoArquivo modo;
  char *path;
};

/* ----------------- DECLARAÇÕES MÉTODOS PRIVADOS ------------------- */

/**
 * Retorna o tamanho da proxima linha do arquivo
 */
static int tamanho_linha(FILE *arq);

/* ----------------- DECLARAÇÕES MÉTODOS PUBLICOS ------------------- */

Arquivo abrir_arquivo(char *path, enum TipoArquivo modo) {
  struct Arquivo *this;
  this = (struct Arquivo *) malloc(sizeof(struct Arquivo));

  switch (modo) {
    case LEITURA: this->arq = fopen(path, "r"); break;
    case ESCRITA: this->arq = fopen(path, "w"); break;
    case ALTERACAO: this->arq = fopen(path, "a"); break;
  }

  if (!this->arq) {
    printf("Nao foi possível abrir o arquivo \"%s\"\n", path);
    free(this);
    exit(EXIT_FAILURE);
  }

  this->linha_atual = 1;
  this->modo        = modo;
  this->path        = (char *) malloc((strlen(path) + 1) * sizeof(char));

  strcpy(this->path, path);

  return (void *) this;
}

char *ler_proxima_linha(Arquivo a) {
  struct Arquivo *this;
  int tam;
  char *linha, *linha_trimmed, c;

  this = (struct Arquivo *) a;

  if (this->modo != LEITURA)
    return NULL;

  tam = tamanho_linha(this->arq);

  // Se estiver no final do arquivo
  if (tam <= 0)
    return NULL;

  // Lembrar de dar free nessa linha;
  linha = (char *) malloc((tam + 1) * sizeof(char));

  // Le a linha até antes do \n
  fgets(linha, tam, this->arq);

  // Pula o \n que ele nao leu
  while ((c = fgetc(this->arq)) != '\n' && c != '\r') {}

  linha_trimmed = trim(linha);

  this->linha_atual++;

  free(linha);

  return linha_trimmed;
}

char *ler_arquivo_inteiro(Arquivo _this) {
  struct Arquivo *this = (struct Arquivo *) _this;
  if (this->modo != LEITURA)
    return NULL;

  // Conta todos os caracteres do arquivo
  int count = 0;
  char c;
  while ((c = fgetc(this->arq)) && !feof(this->arq))
    count++;
  rewind(this->arq);

  char *result = malloc(count + 1);
  for (int i = 0; i <= count; i++)
    result[i] = fgetc(this->arq);
  result[count] = 0;
  rewind(this->arq);

  return result;
}

void escrever_linha(Arquivo a, const char *entrada, ...) {
  struct Arquivo *this;
  va_list args;

  this = (struct Arquivo *) a;

  if (this->modo == LEITURA)
    return;

  va_start(args, entrada);

  vfprintf(this->arq, entrada, args);

  va_end(args);

  this->linha_atual++;
}

int numero_linha_atual(Arquivo a) {
  return ((struct Arquivo *) a)->linha_atual;
}

void fechar_arquivo(Arquivo a) {
  struct Arquivo *this;
  this = (struct Arquivo *) a;

  fclose(this->arq);

  if (this->modo != LEITURA)
    LOG_PRINT(LOG_FILE, "Arquivo \"%s\" salvo com sucesso.", this->path);

  free(this->path);

  free(a);
}

char *get_path(Arquivo a) {
  return ((struct Arquivo *) a)->path;
}

/* ----------------- DEFINIÇÔES MÉTODOS PRIVADOS ------------------- */

int tamanho_linha(FILE *arq) {
  int tam = 0;
  char c;

  while ((c = fgetc(arq)) != '\n' && c != '\r' && !feof(arq)) {
    tam++;
  }

  if (!tam)
    return -1;

  fseek(arq, -(tam + 1), SEEK_CUR);

  return tam + 1;
}
