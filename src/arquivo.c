#include "arquivo.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

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
int tamanho_linha(FILE *arq);

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
    free(this);
    return NULL;
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
  char *linha, c;

  this = (struct Arquivo *) a;

  if (this->modo != LEITURA)
    return NULL;

  tam = tamanho_linha(this->arq);

  /* Se estiver no final do arquivo */
  if (tam <= 0)
    return NULL;

  /* TODO: Lembrar de dar free nessa linha; */
  linha = (char *) malloc((tam + 1) * sizeof(char));

  /* Le a linha até antes do \n */
  fgets(linha, tam, this->arq);

  /* Pula o \n que ele nao leu */

  while ((c = fgetc(this->arq)) != '\n' && c != '\r') {}

  linha = trim(linha);

  this->linha_atual++;

  return linha;
}

void escrever_linha(Arquivo a, char *entrada) {
  struct Arquivo *this;
  this = (struct Arquivo *) a;

  if (this->modo == LEITURA)
    return;

  fputs(entrada, this->arq);
  fputc('\n', this->arq);

  this->linha_atual++;
}

int numero_linha_atual(Arquivo a) {
  return ((struct Arquivo *) a)->linha_atual;
}

void fechar_arquivo(Arquivo a) {
  struct Arquivo *this;
  this = (struct Arquivo *) a;

  fclose(this->arq);

  free(this->path);

  free(a);
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