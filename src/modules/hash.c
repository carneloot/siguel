#include "hash.h"

#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include "logger.h"

#define PORC_TOTALIDADE 0.7
#define SOMA_REALLOC 5

struct HashInfo_t {
  char *chave;
  void *valor;
};

struct HashTable_t {
  int size;
  int count;
  struct HashInfo_t *table;
};

static bool __eh_primo(int numero) {
  if (numero <= 1)
    return false;

  if (numero == 2)
    return true;
  
  if (numero % 2 == 0)
    return false;

  int final = (int) sqrt(numero) + 1;

  for (int den = 3; den < final; den += 2) {
    if (numero % den == 0)
      return false;
  }

  return true;
}

static int __next_primo(int numero) {

  if (numero % 2 == 0)
    numero++;

  while (!__eh_primo(numero))
    numero += 2;

  return numero;
}

HashTable_t ht_create(int tamanho) {
  if (tamanho == 0)
    return NULL;

  assert(tamanho > 0); // Tamanho deve ser positivo

  struct HashTable_t *this = calloc(1, sizeof(*this));

  tamanho = __next_primo(tamanho);

  this->size  = tamanho;
  this->count = 0;
  this->table = calloc(tamanho, sizeof(*this->table));

  return this;
}

void ht_destroy(HashTable_t _this, void (*destruir_item)(void *item), int destruir_chave) {
  struct HashTable_t *this = (struct HashTable_t *) _this;

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

static bool __tem_item_posicao(struct HashInfo_t *table, int posicao) {
  return (table[posicao].chave != NULL);
}

static int hash1(int chave, int size) {
  return (chave % size);
}

static int hash2(int chave, int size) {
  if (size == 1) return 1;
  return (1 + (chave % (size - 1)));
}

static int hash(int chave, int i, int size) {
  return ((hash1(chave, size) + i * hash2(chave, size)) % size);
}

static int char_to_int(char *chave) {
  int soma;
  int tam = strlen(chave);
  soma    = 0;

  for (int i = 0; i < tam; i++) {
    soma += (int) chave[i];
  }

  return soma;
}

static void ht_realocar(struct HashTable_t *this, int new_size) {
  assert(new_size > this->size); // Novo tamanho deve ser maior que o tamanho anterior

  LOG_PRINT(LOG_FILE, "REALOCANDO TABELA COM NOVO TAMANHO DE %d", new_size);

  struct HashInfo_t *nova_tabela = calloc(new_size, sizeof(*nova_tabela));

  for (int i = 0; i < this->size; i++) {
    if (!__tem_item_posicao(this->table, i)) continue;

    struct HashInfo_t info_atual = this->table[i];

    int chave_em_numero = char_to_int(info_atual.chave);

    int j;

    for (j = 0; j < new_size; j++) {
      int posicao = hash(chave_em_numero, j, new_size);

      if (!__tem_item_posicao(nova_tabela, posicao)) {
        nova_tabela[posicao] = info_atual;
        break;
      }
    }

    if (j == new_size) {
      LOG_ERRO("NAO INSERIU O ITEM NA TABELA AO REALOCAR!!");
      exit(1);
    }
  }

  free(this->table);
  this->table = nova_tabela;
  this->size  = new_size;
}

static int ht_passou_limite(struct HashTable_t *this) {
  double porcentagem = ((double) this->count) / ((double) this->size);
  return (porcentagem >= PORC_TOTALIDADE);
}

void ht_insert(HashTable_t _this, char *chave, void *valor) {
  struct HashTable_t *this = (struct HashTable_t *) _this;

  if (!this)
    return;

  int chave_em_numero = char_to_int(chave);

  while (1) {
    int i;
    for (i = 0; i < this->size; i++) {

      int posicao_table = hash(chave_em_numero, i, this->size);

      if (!__tem_item_posicao(this->table, posicao_table)) {
        this->table[posicao_table].chave = chave;
        this->table[posicao_table].valor = valor;
        this->count++;
        break;
      }
    }

    // Nao inseriu
    if (i == this->size) {
      ht_realocar(this, __next_primo(this->size + SOMA_REALLOC));
      continue;
    }

    break;
  }

  // Checar se passou do limite determinado e realocar
  if (ht_passou_limite(this))
    ht_realocar(this, __next_primo(this->size * 2 + 1));
}

int ht_exists(HashTable_t _this, char *chave) {
  struct HashTable_t *this = (struct HashTable_t *) _this;

  if (!this)
    return 0;

  int chave_em_numero = char_to_int(chave);
  
  for (int i = 0; i < this->size; i++) {

    int posicao_table = hash(chave_em_numero, i, this->size);
    
    if (__tem_item_posicao(this->table, posicao_table))
      if (!strcmp(this->table[posicao_table].chave, chave))
        return 1;
  }

  return 0;
}

void *ht_get(HashTable_t _this, char *chave) {
  struct HashTable_t *this = (struct HashTable_t *) _this;

  if (!this)
    return NULL;

  int chave_em_numero = char_to_int(chave);
  for (int i = 0; i < this->size; i++) {

    int posicao_table = hash(chave_em_numero, i, this->size);
    
    if (__tem_item_posicao(this->table, posicao_table)) {
      if (strcmp(this->table[posicao_table].chave, chave))
        continue;
      return this->table[posicao_table].valor;
    }
  }

  return NULL;
}

void ht_remove(HashTable_t _this, char *chave) {
  struct HashTable_t *this = (struct HashTable_t *) _this;

  if (!this)
    return;

  int chave_em_numero = char_to_int(chave);
  for (int i = 0; i < this->size; i++) {

    int posicao_table = hash(chave_em_numero, i, this->size);

    if (__tem_item_posicao(this->table, posicao_table)) {
      if (strcmp(this->table[posicao_table].chave, chave))
        continue;
      this->table[posicao_table].chave = NULL;
      this->table[posicao_table].valor = NULL;
      this->count--;
      break;
    }
  }
}

void ht_print(
  HashTable_t _this, char *(*to_string)(void *valor), FILE *fp) {
  struct HashTable_t *this = (struct HashTable_t *) _this;

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

void ht_map(
  HashTable_t _this, void *other, void (*map_function)(void *valor, void *other)) {
  struct HashTable_t *this = (struct HashTable_t *) _this;

  for (int i = 0; i < this->size; i++) {
    if (this->table[i].chave == NULL) continue;

    map_function(this->table[i].valor, other);
  }
}

int ht_length(HashTable_t _this) {
  struct HashTable_t * this = (struct HashTable_t *) _this;

  if (!this)
    return 0;

  return this->count;
}

int ht_max_size(HashTable_t _this) {
  struct HashTable_t * this = (struct HashTable_t *) _this;

  if (!this)
    return 0;

  return this->size;
}
