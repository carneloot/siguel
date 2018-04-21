#include "arquivo.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Arquivo {
  FILE *arq;
  int linha_atual;
  enum TipoArquivo modo;
  char *path;
};

// ----------------- DECLARAÇÕES MÉTODOS PRIVADOS -------------------

/**
 * Retorna o tamanho da proxima linha do arquivo
 */
int tamanho_linha(FILE *arq);

// ----------------- DECLARAÇÕES MÉTODOS PUBLICOS -------------------

Arquivo abrir_arquivo(char *path, enum TipoArquivo modo) {
  struct Arquivo *this = (struct Arquivo *) malloc(sizeof(struct Arquivo));

  switch (modo) {
    case LEITURA: this->arq = fopen(path, "r"); break;
    case ESCRITA: this->arq = fopen(path, "w"); break;
    case ALTERACAO: this->arq = fopen(path, "a"); break;
  }

  this->linha_atual = 1;
  this->modo        = modo;
  this->path        = (char *) malloc((strlen(path) + 1) * sizeof(char));

  strcpy(this->path, path);

  return (void *) this;
}

char *ler_proxima_linha(Arquivo a) {
  struct Arquivo *this = (struct Arquivo *) a;

  if (this->modo != LEITURA)
    return NULL;

  int tam = tamanho_linha(this->arq);

  // Se estiver no final do arquivo
  if (tam <= 0)
    return NULL;

  // TODO: Lembrar de dar free nessa linha;
  char *linha = (char *) malloc(tam * sizeof(char));

  // Le a linha até antes do \n
  fgets(linha, tam, this->arq);

  // Pula o \n que ele nao leu
  fseek(this->arq, 1, SEEK_CUR);

  this->linha_atual++;

  return linha;
}

void escrever_linha(Arquivo a, char *entrada) {
  struct Arquivo *this = (struct Arquivo *) a;

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
  struct Arquivo *this = (struct Arquivo *) a;

  fclose(this->arq);

  free(this->path);

  free(a);
}

char *get_nome(Arquivo a) {
  return "";
}

char *get_diretorio(Arquivo a) {
  return "";
}

char *get_extensao(Arquivo a) {
  return "";
}

// ----------------- DEFINIÇÔES MÉTODOS PRIVADOS -------------------

int tamanho_linha(FILE *arq) {
  int tam = 0;
  char c;

  while ((c = fgetc(arq)) != '\n' && !feof(arq)) {
    tam++;
  }

  if (!tam)
    return -1;

  fseek(arq, -(tam + 1), SEEK_CUR);

  return tam;
}