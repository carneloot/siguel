#include "hash.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define PORC_TOTALIDADE 0.7
#define SOMA_REALLOC 5

struct HashTable {
  unsigned size;
  unsigned count;
  HashInfo *table;
};

static HashTable __create_hashtable(int tamanho) {
  if (tamanho == 0)
    return NULL;

  assert(tamanho > 0); // Tamanho deve ser positivo

  struct HashTable *this = calloc(1, sizeof(*this));

  this->size  = tamanho;
  this->count = 0;
  this->table = calloc(tamanho, sizeof(*this->table));

  return this;
}

static void __destroy_hashtable(HashTable _this, void (*destruir_item)(void *item), int destruir_chave) {
  struct HashTable *this = (struct HashTable *) _this;

  if (!this)
    return;

  if (destruir_item) {
    for (int i = 0; i < this->size; i++) {
      if (!this->table[i].valor) continue;

      destruir_item(this->table[i].valor);
      this->table[i].valor = NULL;
    }
  }

  if (destruir_chave) {
    for (int i = 0; i < this->size; i++) {
      if (!this->table[i].chave) continue;

      free(this->table[i].chave);
      this->table[i].chave = NULL;
    }
  }

  free(this->table);
  free(this);
}

static unsigned hash1(unsigned chave, unsigned size) {
  return (chave % size);
}

static unsigned hash2(unsigned chave, unsigned size) {
  if (size == 1) return 1;
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

static void __realocar_hashtable(struct HashTable *this, unsigned new_size) {
  assert(new_size > this->size); // Novo tamanho deve ser maior que o tamanho anterior

  HashInfo *nova_tabela = calloc(new_size, sizeof(*nova_tabela));

  for (int i = 0; i < this->size; i++) {
    HashInfo info_atual = this->table[i];
    if (!info_atual.chave || !info_atual.valor) continue;

    unsigned chave_em_numero = char_to_unsigned(info_atual.chave);
    for (int j = 0; j < new_size; j++) {
      unsigned posicao = hash(chave_em_numero, j, new_size);

      if (!nova_tabela[posicao].valor) {
        nova_tabela[posicao] = info_atual;
        break;
      }
    }
  }

  free(this->table);
  this->table = nova_tabela;
  this->size  = new_size;
}

static int __passou_limite_hashtable(struct HashTable *this) {
  double porcentagem = ((double) this->count) / ((double) this->size);
  return (porcentagem >= PORC_TOTALIDADE);
}

static void __insert_hashtable(HashTable _this, HashInfo info) {
  struct HashTable *this = (struct HashTable *) _this;

  if (!this)
    return;

  unsigned chave_em_numero = char_to_unsigned(info.chave);

  while (1) {
    int i;
    for (i = 0; i < this->size; i++) {

      int posicao_table = hash(chave_em_numero, i, this->size);

      if (!this->table[posicao_table].valor) {
        this->table[posicao_table] = info;
        this->count++;
        break;
      }
    }

    // Nao inseriu
    if (i == this->size) {
      __realocar_hashtable(this, this->size + SOMA_REALLOC);
      continue;
    }

    break;
  }

  // Checar se passou do limite determinado e realocar
  if (__passou_limite_hashtable(this))
    __realocar_hashtable(this, this->size * 2);
}

static int __exists_hashtable(HashTable _this, const char chave[]) {
  struct HashTable *this = (struct HashTable *) _this;

  if (!this)
    return 0;

  unsigned chave_em_numero = char_to_unsigned(chave);
  
  for (int i = 0; i < this->size; i++) {

    int posicao_table = hash(chave_em_numero, i, this->size);
    
    HashInfo info = this->table[posicao_table];

    if (info.valor)
      if (!strcmp(info.chave, chave))
        return 1;
  }

  return 0;
}

static HashInfo __get_hashtable(HashTable _this, const char chave[]) {
  struct HashTable *this = (struct HashTable *) _this;

  HashInfo nulo = {.chave = NULL, .valor = NULL};

  if (!this)
    return nulo;

  unsigned chave_em_numero = char_to_unsigned(chave);
  for (int i = 0; i < this->size; i++) {

    int posicao_table = hash(chave_em_numero, i, this->size);
    
    HashInfo info = this->table[posicao_table];

    if (info.valor) {
      if (strcmp(info.chave, chave))
        continue;
      return info;
    }
  }

  return nulo;
}

static void __remove_hashtable(HashTable _this, const char chave[]) {
  struct HashTable *this = (struct HashTable *) _this;

  if (!this)
    return;

  unsigned chave_em_numero = char_to_unsigned(chave);
  for (int i = 0; i < this->size; i++) {

    int posicao_table = hash(chave_em_numero, i, this->size);

    if (this->table[posicao_table].valor) {
      if (strcmp(this->table[posicao_table].chave, chave))
        continue;
      this->table[posicao_table].chave = NULL;
      this->table[posicao_table].valor = NULL;
      this->count--;
      break;
    }
  }
}

static void __print_hashtable(
  HashTable _this, char *(*to_string)(void *valor), FILE *fp) {
  struct HashTable *this = (struct HashTable *) _this;

  if (!this)
    return;

  for (int i = 0; i < this->size; i++) {
    if (!this->table[i].valor)
      continue;
    char *string_valor = to_string(this->table[i].valor);
    fprintf(fp, "%d: (%s)\t%s\n", i, this->table[i].chave, string_valor);
    free(string_valor);
  }
}

static void __map_hashtable(
  HashTable _this, void *other, void (*map_function)(const void *valor, void *other)) {
  struct HashTable *this = (struct HashTable *) _this;

  for (int i = 0; i < this->size; i++) {
    if (this->table[i].chave == NULL) continue;

    map_function(this->table[i].valor, other);
  }
}

static unsigned __length_hashtable(HashTable _this) {
  struct HashTable * this = (struct HashTable *) _this;

  if (!this)
    return 0;

  return this->count;
}

static unsigned __max_size_hashtable(HashTable _this) {
  struct HashTable * this = (struct HashTable *) _this;

  if (!this)
    return 0;

  return this->size;
}

const struct HashTable_t HashTable_t = {  //
  .create   = &__create_hashtable,
  .destroy  = &__destroy_hashtable,
  .insert   = &__insert_hashtable,
  .exists   = &__exists_hashtable,
  .get      = &__get_hashtable,
  .remove   = &__remove_hashtable,
  .print    = &__print_hashtable,
  .length   = &__length_hashtable,
  .max_size = &__max_size_hashtable,
  .map      = &__map_hashtable,
};