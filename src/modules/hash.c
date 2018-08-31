#include "hash.h"

#include <stdlib.h>
#include <string.h>

struct HashTable {
  unsigned size;
  HashInfo *table;
};

HashTable __create_hashtable(unsigned tamanho) {
  struct HashTable *this = calloc(1, sizeof(*this));

  this->size  = tamanho;
  this->table = calloc(tamanho, sizeof(*this->table));

  return this;
}

void __destroy_hashtable(HashTable _this, void (*destruir_item)(void *item)) {
  struct HashTable *this = (struct HashTable *) _this;

  if (destruir_item) {
    for (int i = 0; i < this->size; i++) {
      if (!this->table[i].valor) continue;

      destruir_item(this->table[i].valor);
    }
  }

  free(this->table);
  free(this);
}

static unsigned hash1(unsigned chave, unsigned size) {
  return (chave % size);
}

static unsigned hash2(unsigned chave, unsigned size) {
  return (1 + (chave % (size - 1)));
}

static unsigned hash(unsigned chave, unsigned i, unsigned size) {
  return ((hash1(chave, size) + i * hash2(chave, size)) % size);
}

static unsigned char_to_unsigned(const char chave[]) {
  unsigned soma;
  int tam = strlen(chave);
  soma    = 0;

  for (int i = 0; i < tam; i++) {
    soma += (int) chave[i];
  }

  return soma;
}

void __insert_hashtable(HashTable _this, HashInfo info) {
  struct HashTable *this = (struct HashTable *) _this;

  for (int i = 0; i < this->size; i++) {
    unsigned chave_em_numero = char_to_unsigned(info.chave);

    int posicao_table = hash(chave_em_numero, i, this->size);

    if (!this->table[posicao_table].valor) {
      this->table[posicao_table] = info;
      break;
    }
  }
}

int __exists_hashtable(HashTable _this, const char chave[]) {
  struct HashTable *this = (struct HashTable *) _this;

  for (int i = 0; i < this->size; i++) {
    unsigned chave_em_numero = char_to_unsigned(chave);

    int posicao_table = hash(chave_em_numero, i, this->size);
    
    HashInfo info = this->table[posicao_table];

    if (info.valor)
      if (!strcmp(info.chave, chave))
        return 1;
  }

  return 0;
}

HashInfo __get_hashtable(HashTable _this, const char chave[]) {
  struct HashTable *this = (struct HashTable *) _this;

  for (int i = 0; i < this->size; i++) {
    unsigned chave_em_numero = char_to_unsigned(chave);

    int posicao_table = hash(chave_em_numero, i, this->size);
    
    HashInfo info = this->table[posicao_table];

    if (info.valor) {
      if (strcmp(info.chave, chave))
        continue;
      return info;
    }
  }

  HashInfo retorno = {.chave = NULL, .valor = NULL};

  return retorno;
}

void __remove_hashtable(HashTable _this, const char chave[]) {
  struct HashTable *this = (struct HashTable *) _this;

  for (int i = 0; i < this->size; i++) {
    unsigned chave_em_numero = char_to_unsigned(chave);

    int posicao_table = hash(chave_em_numero, i, this->size);

    if (this->table[posicao_table].valor) {
      if (strcmp(this->table[posicao_table].chave, chave))
        continue;
      this->table[posicao_table].chave = NULL;
      this->table[posicao_table].valor = NULL;
      break;
    }
  }
}

void __print_hashtable(
  HashTable _this, char *(*to_string)(void *valor), FILE *fp) {
  struct HashTable *this = (struct HashTable *) _this;

  for (int i = 0; i < this->size; i++) {
    if (!this->table[i].valor)
      continue;
    char *string_valor = to_string(this->table[i].valor);
    fprintf(fp, "%d: (%s)\t%s\n", i, this->table[i].chave, string_valor);
    free(string_valor);
  }
}

const struct HashTable_t HashTable_t = {  //
  .create  = &__create_hashtable,
  .destroy = &__destroy_hashtable,
  .insert  = &__insert_hashtable,
  .exists  = &__exists_hashtable,
  .get     = &__get_hashtable,
  .remove  = &__remove_hashtable,
  .print   = &__print_hashtable};