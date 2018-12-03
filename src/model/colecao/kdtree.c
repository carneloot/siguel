#include <math.h>
#include <stdlib.h>

#include "kdtree.h"

typedef int (*func_compare)(void *a, void *b);

struct KDTree_t {
  void *value;
  struct KDTree_t *left, *right;

  unsigned qtd;

  int dim;
  func_compare check_equal;
  func_compare *funcs;
};

// ===== FUNCOES =====

static KDTree_t kdt_create_aux(
  int dim, func_compare check_equal, func_compare *funcs) {
  struct KDTree_t *this = calloc(1, sizeof(struct KDTree_t));

  this->dim   = dim;
  this->value = NULL;
  this->left  = NULL;
  this->right = NULL;

  this->check_equal = check_equal;

  this->funcs = funcs;

  this->qtd = 0;

  return this;
}

KDTree_t kdt_create(int dim, func_compare check_equal, ...) {
  func_compare *funcs = calloc(dim, sizeof(func_compare));

  va_list list;
  va_start(list, check_equal);

  for (int i = 0; i < dim; i++)
    funcs[i] = va_arg(list, func_compare);

  va_end(list);

  return kdt_create_aux(dim, check_equal, funcs);
}

int kdt_is_leaf(KDTree_t _this) {
  struct KDTree_t *this = (struct KDTree_t *) _this;
  return !(this->left || this->right);
}

int kdt_is_empty(KDTree_t _this) {
  struct KDTree_t *this = (struct KDTree_t *) _this;
  return !(this->value);
}

static KDTree_t kdt_minimum(KDTree_t _a, KDTree_t _b, KDTree_t _c, unsigned dim) {
  struct KDTree_t *a = (struct KDTree_t *) _a;
  struct KDTree_t *b = (struct KDTree_t *) _b;
  struct KDTree_t *c = (struct KDTree_t *) _c;

  struct KDTree_t *min  = a;
  func_compare funcao = a->funcs[dim];
  if (b && funcao(b->value, min->value) < 0)
    min = b;
  if (c && funcao(c->value, min->value) < 0)
    min = c;

  return min;
}

static KDTree_t kdt_find_min_rec(KDTree_t _this, unsigned dim, unsigned prof) {
  struct KDTree_t *this = (struct KDTree_t *) _this;

  if (!_this)
    return NULL;

  unsigned cd = prof % this->dim;

  // Só pode estar a esquerda se a dimensao for a mesma
  if (dim == cd) {
    if (!this->left)
      return this;
    return kdt_find_min_rec(this->left, dim, prof + 1);
  }

  // Pode estar em qualquer lado
  return kdt_minimum(
    this,
    kdt_find_min_rec(this->left, dim, prof + 1),
    kdt_find_min_rec(this->right, dim, prof + 1),
    dim);
}

static KDTree_t kdt_find_min(KDTree_t this, unsigned dim) {
  return kdt_find_min_rec(this, dim, 0);
}

static KDTree_t kdt_insert_rec(KDTree_t _this, void *value, unsigned prof) {
  struct KDTree_t *this = (struct KDTree_t *) _this;

  this->qtd++;

  if (this->value == NULL) {
    this->value = value;
    return _this;
  }

  int indice_func = prof % this->dim;

  if (this->funcs[indice_func](value, this->value) < 0) {
    if (!this->left)
      this->left =
        kdt_create_aux(this->dim, this->check_equal, this->funcs);

    return kdt_insert_rec(this->left, value, prof + 1);
  } else {
    if (!this->right)
      this->right =
        kdt_create_aux(this->dim, this->check_equal, this->funcs);

    return kdt_insert_rec(this->right, value, prof + 1);
  }

  return NULL;
}

KDTree_t kdt_insert(KDTree_t this, void *value) {
  return kdt_insert_rec(this, value, 0);
}

static KDTree_t kdt_remove_rec(KDTree_t _this, void *value, unsigned prof) {
  struct KDTree_t *this = (struct KDTree_t *) _this;

  if (!this)
    return NULL;

  this->qtd--;

  unsigned cd = prof % this->dim;

  // Se nao estiver nesse node
  if (!this->check_equal(this->value, value)) {
    if (this->funcs[cd](value, this->value) < 0)
      this->left = kdt_remove_rec(this->left, value, prof + 1);
    else
      this->right = kdt_remove_rec(this->right, value, prof + 1);
    return this;
  }

  // Se estiver nesse node
  if (kdt_is_leaf(this)) {
    if (prof != 0)
      free(this);
    else
      this->value = NULL;
    return NULL;
  }

  // Se contem no a direita
  if (this->right) {
    struct KDTree_t *min;
    min         = kdt_find_min(this->right, cd);
    this->value = min->value;
    this->right = kdt_remove_rec(this->right, this->value, prof + 1);
  }

  // Se contem no a esquerda
  else if (this->left) {
    struct KDTree_t *min;
    min         = kdt_find_min(this->left, cd);
    this->value = min->value;
    this->right = kdt_remove_rec(this->left, this->value, prof + 1);
    this->left  = NULL;
  }

  return this;
}

void kdt_remove(KDTree_t this, void *value) {
  kdt_remove_rec(this, value, 0);
}

static KDTree_t kdt_search_rec(
  KDTree_t _this,
  void *value,
  int (*check)(void *a, void *b),
  unsigned prof) {
  struct KDTree_t *this = (struct KDTree_t *) _this;

  if (!this)
    return NULL;

  if (check(this->value, value))
    return this;

  int indice_func = prof % this->dim;

  if (this->funcs[indice_func](value, this->value) < 0)
    return kdt_search_rec(this->left, value, check, prof + 1);
  else
    return kdt_search_rec(this->right, value, check, prof + 1);

  return NULL;
}

KDTree_t kdt_search(
  KDTree_t this, void *value, int (*check)(void *a, void *b)) {
  return kdt_search_rec(this, value, check, 0);
}

void *kdt_get(KDTree_t _this) {
  struct KDTree_t *this = (struct KDTree_t *) _this;
  return this->value;
}

static void kdt_passe_simetrico_rec(
  KDTree_t _this,
  void (*executar)(void *item, unsigned prof, va_list list),
  unsigned prof,
  va_list _list) {
  struct KDTree_t *this = (struct KDTree_t *) _this;

  if (this->left)
    kdt_passe_simetrico_rec(this->left, executar, prof + 1, _list);

  if (this->value) {
    va_list list;
    va_copy(list, _list);

    executar(this->value, prof, list);

    va_end(list);
  }

  if (this->right)
    kdt_passe_simetrico_rec(this->right, executar, prof + 1, _list);
}

void kdt_passe_simetrico(
  KDTree_t this,
  void (*executar)(void *item, unsigned prof, va_list list),
  ...) {
  va_list list;
  va_start(list, executar);
  kdt_passe_simetrico_rec(this, executar, 0, list);
  va_end(list);
}

typedef int (*__range_search_dentro)(void *value, int dim, void *ponto_a, void *ponto_b);

static void kdt_range_search_rec(
  KDTree_t _this,
  Lista_t saida,
  void *ponto_a, void *ponto_b,
  __range_search_dentro dentro,
  unsigned prof) {
  struct KDTree_t *this = (struct KDTree_t *) _this;

  if (!this)
    return;

  int local;
  unsigned cd;

  // Checa a posicao do atributo 'cd' em relacao com a area
  cd    = prof % this->dim;
  local = dentro(this->value, cd, ponto_a, ponto_b);

  // Esquerda
  if (local < 0)
    kdt_range_search_rec(this->right, saida, ponto_a, ponto_b, dentro, prof + 1);

  // Direita
  else if (local > 0)
    kdt_range_search_rec(this->left, saida, ponto_a, ponto_b, dentro, prof + 1);

  // Dentro
  else {
    kdt_range_search_rec(this->left, saida, ponto_a, ponto_b, dentro, prof + 1);
    kdt_range_search_rec(this->right, saida, ponto_a, ponto_b, dentro, prof + 1);
  }

  if (dentro(this->value, -1, ponto_a, ponto_b))  // Se o valor esta dentro do retangulo
    lt_insert(saida, this->value);
}

Lista_t kdt_range_search(
  KDTree_t _this, __range_search_dentro dentro, void *ponto_a, void *ponto_b) {
  struct KDTree_t *this = (struct KDTree_t *) _this;

  Lista_t saida;

  saida = lt_create();

  kdt_range_search_rec(this, saida, ponto_a, ponto_b, dentro, 0);

  return saida;
}

static struct KDTree_t *__closest;
static double __closest_distance;

static void kdt_nearest_neighbor_rec(
  struct KDTree_t *this,
  void *value,
  double (*distance)(void *a, void *b, int dim),
  unsigned prof) {

  if (!this)
    return;

  double distancia = distance(this->value, value, -1);

  if (__closest == NULL || distancia < __closest_distance) {
    __closest = this;
    __closest_distance = distancia;
  }

  int currDim = prof % this->dim;

  struct KDTree_t *other;

  if (this->funcs[currDim](value, this->value) < 0) {
    kdt_nearest_neighbor_rec(this->left, value, distance, prof + 1);
    other = this->right;
  } else {
    kdt_nearest_neighbor_rec(this->right, value, distance, prof + 1);
    other = this->left;
  }

  double distanciaDimensao = distance(this->value, value, currDim);

  if (distanciaDimensao < __closest_distance)
    kdt_nearest_neighbor_rec(other, value, distance, prof + 1);
}

Pair kdt_nearest_neighbor(
  KDTree_t _this,
  void *value,
  double (*distance)(void *a, void *b, int dim)) {
  struct KDTree_t *this = (struct KDTree_t *) _this;
  Pair result;

  __closest          = NULL;
  __closest_distance = INFINITY;
  kdt_nearest_neighbor_rec(this, value, distance, 0);

  result.distance = __closest_distance;
  result.point1   = (__closest) ? __closest->value : NULL;
  result.point2   = value;

  return result;
}

static Pair kdt_closest_pair_rec(
  KDTree_t _raiz,
  KDTree_t _it,
  Pair entrada,
  double (*distance)(void *a, void *b, int dim)) {
  struct KDTree_t *raiz = (struct KDTree_t *) _raiz;
  struct KDTree_t *it   = (struct KDTree_t *) _it;

  Pair result;

  result = kdt_nearest_neighbor(raiz, it->value, distance);

  if (it->left)
    result = kdt_closest_pair_rec(raiz, it->left, result, distance);

  if (it->right)
    result = kdt_closest_pair_rec(raiz, it->right, result, distance);

  return (entrada.distance < result.distance) ? entrada : result;
}

Pair kdt_closest_pair(
  KDTree_t _this, double (*distance)(void *a, void *b, int dim)) {
  struct KDTree_t *this = (struct KDTree_t *) _this;

  Pair result = {
    .distance = INFINITY,
    .point1   = NULL,
    .point2   = NULL,
  };

  return kdt_closest_pair_rec(this, this, result, distance);
}

static void kdt_generate_dot_rec(
  struct KDTree_t *this, FILE *fp, char *(*to_string)(void *item), int prof) {

  if (this->value) {
    char *strA = to_string(this->value);
    fprintf(
      fp, "\"%p\" [label=\"%d\\n%s\"];\n", this->value, prof % this->dim, strA);
    free(strA);
  }

  struct KDTree_t *other = this->left;

  for (int i = 0; i < 2; i++) {
    if (other) {
      char *strB = to_string(other->value);

      fprintf(fp, "\"%p\" -> \"%p\";\n", this->value, other->value);
      kdt_generate_dot_rec(other, fp, to_string, prof + 1);

      free(strB);
    } else {
      fprintf(fp, "\"n%p\" [label=\"nil\",style=invis];\n", this->value);
      fprintf(
        fp, "\"%p\" -> \"n%p\"[style=invis];\n", this->value, this->value);
    }
    other = this->right;
  }

}

void kdt_generate_dot(
  KDTree_t _this, FILE *fp, char *(*to_string)(void *item)) {
  struct KDTree_t *this = (struct KDTree_t *) _this;
  fprintf(fp, "digraph T {\n");
  fprintf(fp, "node [fontname=\"Arial\"];\n");
  fprintf(fp, "\"root\" -> \"%p\";\n", (this->value) ? this->value : "");
  kdt_generate_dot_rec(this, fp, to_string, 0);
  fprintf(fp, "}\n");
}

static void kdt_destroy_rec(KDTree_t _this, void (*destroy)(void *item)) {
  struct KDTree_t *this = (struct KDTree_t *) _this;

  if (this->left)
    kdt_destroy_rec(this->left, destroy);

  if (this->right)
    kdt_destroy_rec(this->right, destroy);

  if (destroy && this->value)
    destroy(this->value);

  free(this);
}

void kdt_destroy(KDTree_t _this, void (*destroy)(void *item)) {
  struct KDTree_t *this = (struct KDTree_t *) _this;

  free(this->funcs);

  kdt_destroy_rec(this, destroy);
}
