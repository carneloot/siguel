#include <math.h>
#include <stdlib.h>

#include "kdtree.h"

typedef int (*func_compare)(const void *a, const void *b);

struct KDTree {
  Item value;
  struct KDTree *left, *right;

  int dim;
  func_compare check_equal;
  func_compare *funcs;
};

// ===== FUNCOES =====

static KDTree __create_aux_kdtree(
  int dim, func_compare check_equal, func_compare *funcs) {
  struct KDTree *this = calloc(1, sizeof(struct KDTree));

  this->dim   = dim;
  this->value = NULL;
  this->left  = NULL;
  this->right = NULL;

  this->check_equal = check_equal;

  this->funcs = funcs;

  return this;
}

static KDTree __create_kdtree(int dim, func_compare check_equal, ...) {
  func_compare *funcs = calloc(dim, sizeof(func_compare));

  va_list list;
  va_start(list, check_equal);

  for (int i = 0; i < dim; i++)
    funcs[i] = va_arg(list, func_compare);

  va_end(list);

  return (KDTree) __create_aux_kdtree(dim, check_equal, funcs);
}

static int __is_leaf_kdtree(KDTree _this) {
  struct KDTree *this = (struct KDTree *) _this;
  return !(this->left || this->right);
}

static int __is_empty_kdtree(KDTree _this) {
  struct KDTree *this = (struct KDTree *) _this;
  return !(this->value);
}

static KDTree __minimum_kdtree(KDTree _a, KDTree _b, KDTree _c, unsigned dim) {
  struct KDTree *a = (struct KDTree *) _a;
  struct KDTree *b = (struct KDTree *) _b;
  struct KDTree *c = (struct KDTree *) _c;

  struct KDTree *min  = a;
  func_compare funcao = a->funcs[dim];
  if (b && funcao(b->value, min->value) < 0)
    min = b;
  if (c && funcao(c->value, min->value) < 0)
    min = c;

  return min;
}

static KDTree __find_min_rec_kdtree(KDTree _this, unsigned dim, unsigned prof) {
  struct KDTree *this = (struct KDTree *) _this;

  if (!_this)
    return NULL;

  unsigned cd = prof % this->dim;

  // Só pode estar a esquerda se a dimensao for a mesma
  if (dim == cd) {
    if (!this->left)
      return this;
    return __find_min_rec_kdtree(this->left, dim, prof + 1);
  }

  // Pode estar em qualquer lado
  return __minimum_kdtree(
    this,
    __find_min_rec_kdtree(this->left, dim, prof + 1),
    __find_min_rec_kdtree(this->right, dim, prof + 1),
    dim);
}

static KDTree __find_min_kdtree(KDTree this, unsigned dim) {
  return __find_min_rec_kdtree(this, dim, 0);
}

static KDTree __insert_rec_kdtree(KDTree _this, Item value, unsigned prof) {
  struct KDTree *this = (struct KDTree *) _this;

  if (this->value == NULL) {
    this->value = value;
    return _this;
  }

  // Se o ponto for o mesmo, nao insere
  if (this->check_equal(value, this->value))
    return _this;

  int indice_func = prof % this->dim;

  if (this->funcs[indice_func](value, this->value) < 0) {
    if (!this->left)
      this->left =
        __create_aux_kdtree(this->dim, this->check_equal, this->funcs);

    return __insert_rec_kdtree(this->left, value, prof + 1);
  } else {
    if (!this->right)
      this->right =
        __create_aux_kdtree(this->dim, this->check_equal, this->funcs);

    return __insert_rec_kdtree(this->right, value, prof + 1);
  }

  return NULL;
}

static KDTree __insert_kdtree(KDTree this, Item value) {
  return __insert_rec_kdtree(this, value, 0);
}

static KDTree __delete_rec_kdtree(KDTree _this, Item value, unsigned prof) {
  struct KDTree *this = (struct KDTree *) _this;

  // Se nao estiver nesse node
  if (!this->check_equal(this->value, value)) {
    if (this->funcs[prof % this->dim](value, this->value) < 0)
      this->left = __delete_rec_kdtree(this->left, value, prof + 1);
    else
      this->right = __delete_rec_kdtree(this->right, value, prof + 1);
    return this;
  }

  // Se estiver nesse node
  if (__is_leaf_kdtree(this)) {
    free(this);
    return NULL;
  }

  // Se nao estiver nesse node
  struct KDTree *min;
  unsigned cd = prof % this->dim;

  // Se contem no a direita
  if (this->right) {
    min         = __find_min_kdtree(this->right, cd);
    this->value = min->value;
    this->right = __delete_rec_kdtree(this->right, min->value, prof + 1);
  }

  // Se contem no a esquerda
  else if (this->left) {
    min         = __find_min_kdtree(this->left, cd);
    this->value = min->value;
    this->left  = __delete_rec_kdtree(this->left, min->value, prof + 1);
    this->right = this->left;
    this->left  = NULL;
  }

  return this;
}

static KDTree __delete_kdtree(KDTree this, Item value) {
  return __delete_rec_kdtree(this, value, 0);
}

static KDTree __search_rec_kdtree(
  KDTree _this,
  Item value,
  int (*check)(const void *a, const void *b),
  unsigned prof) {
  struct KDTree *this = (struct KDTree *) _this;

  if (!this)
    return NULL;

  if (check(this->value, value))
    return this;

  int indice_func = prof % this->dim;

  if (this->funcs[indice_func](value, this->value) < 0)
    return __search_rec_kdtree(this->left, value, check, prof + 1);
  else
    return __search_rec_kdtree(this->right, value, check, prof + 1);

  return NULL;
}

static KDTree __search_kdtree(
  KDTree this, Item value, int (*check)(const void *a, const void *b)) {
  return __search_rec_kdtree(this, value, check, 0);
}

static Item __get_kdtree(KDTree _this) {
  struct KDTree *this = (struct KDTree *) _this;
  return this->value;
}

static void __passe_simetrico_rec_kdtree(
  KDTree _this,
  void (*executar)(const Item item, unsigned prof, va_list list),
  unsigned prof,
  va_list list) {
  struct KDTree *this = (struct KDTree *) _this;

  if (this->left)
    __passe_simetrico_rec_kdtree(this->left, executar, prof + 1, list);

  executar(this->value, prof, list);

  if (this->right)
    __passe_simetrico_rec_kdtree(this->right, executar, prof + 1, list);
}

static void __passe_simetrico_kdtree(
  KDTree this,
  void (*executar)(const Item item, unsigned prof, va_list list),
  ...) {
  va_list list;
  va_start(list, executar);
  __passe_simetrico_rec_kdtree(this, executar, 0, list);
  va_end(list);
}

static void __range_search_rec_kdtree(
  KDTree _this,
  Lista saida,
  Item rect[],
  int (*dentro)(Item value, int dim, Item rect[]),
  unsigned prof) {
  struct KDTree *this = (struct KDTree *) _this;

  if (!this)
    return;

  int local;
  unsigned cd;

  // Checa a posicao do atributo 'cd' em relacao com a area
  cd    = prof % this->dim;
  local = dentro(this->value, cd, rect);

  // Esquerda
  if (local < 0)
    __range_search_rec_kdtree(this->right, saida, rect, dentro, prof + 1);

  // Direita
  else if (local > 0)
    __range_search_rec_kdtree(this->left, saida, rect, dentro, prof + 1);

  // Dentro
  else {
    __range_search_rec_kdtree(this->left, saida, rect, dentro, prof + 1);
    __range_search_rec_kdtree(this->right, saida, rect, dentro, prof + 1);
  }

  if (dentro(this->value, -1, rect))  // Se o valor esta dentro do ponto
    Lista_t.insert(saida, this->value);
}

static Lista __range_search_kdtree(
  KDTree _this, int (*dentro)(Item value, int dim, Item rect[]), ...) {
  struct KDTree *this = (struct KDTree *) _this;

  Lista saida;
  Item *rect;
  va_list list;

  saida = Lista_t.create();

  rect = calloc(this->dim, sizeof(Item));

  va_start(list, dentro);

  for (int i = 0; i < this->dim; i++)
    rect[i] = va_arg(list, Item);

  va_end(list);

  __range_search_rec_kdtree(this, saida, rect, dentro, 0);

  free(rect);

  return saida;
}

static struct KDTree *__closest;
static double __closest_distance;

static void __nearest_neighbor_rec_kdtree(
  KDTree _this,
  Item value,
  double (*distance)(const Item a, const Item b, int dim),
  unsigned prof) {
  struct KDTree *this = (struct KDTree *) _this;

  if (!this)
    return;

  if (this->value == value)
    return;

  unsigned cd = prof % this->dim;

  KDTree other = NULL;

  if (!__closest) {
    if (__is_leaf_kdtree(this)) {
      __closest          = this;
      __closest_distance = distance(this->value, value, -1);
      return;
    }

    if (this->funcs[cd](value, this->value) < 0) {  // Esquerda
      __nearest_neighbor_rec_kdtree(this->left, value, distance, prof + 1);
      other = this->right;
    } else {  // Direita
      __nearest_neighbor_rec_kdtree(this->right, value, distance, prof + 1);
      other = this->left;
    }

    if (!__closest)
      __nearest_neighbor_rec_kdtree(other, value, distance, prof + 1);
  }

  double dist_atual, dist_dimensao;

  dist_atual = distance(this->value, value, -1);

  if (dist_atual < __closest_distance) {
    __closest          = this;
    __closest_distance = dist_atual;
  }

  dist_dimensao = distance(this->value, value, cd);

  // Cruza a linha que quebra
  if (dist_dimensao < __closest_distance)
    __nearest_neighbor_rec_kdtree(other, value, distance, prof + 1);
}

static Pair __nearest_neighbor_kdtree(
  KDTree _this,
  Item value,
  double (*distance)(const Item a, const Item b, int dim)) {
  struct KDTree *this = (struct KDTree *) _this;
  Pair result;

  __closest          = NULL;
  __closest_distance = INFINITY;
  __nearest_neighbor_rec_kdtree(this, value, distance, 0);

  result.distance = __closest_distance;
  result.point1   = (__closest) ? __closest->value : NULL;
  result.point2   = value;

  return result;
}

static Pair __closest_pair_rec_kdtree(
  KDTree _raiz,
  KDTree _it,
  Pair entrada,
  double (*distance)(const Item a, const Item b, int dim)) {
  struct KDTree *raiz = (struct KDTree *) _raiz;
  struct KDTree *it   = (struct KDTree *) _it;

  Pair result;

  result = __nearest_neighbor_kdtree(raiz, it->value, distance);

  if (it->left)
    result = __closest_pair_rec_kdtree(raiz, it->left, result, distance);

  if (it->right)
    result = __closest_pair_rec_kdtree(raiz, it->right, result, distance);

  return (entrada.distance < result.distance) ? entrada : result;
}

static Pair __closest_pair_kdtree(
  KDTree _this, double (*distance)(const Item a, const Item b, int dim)) {
  struct KDTree *this = (struct KDTree *) _this;

  Pair result = {
    .distance = INFINITY,
    .point1   = NULL,
    .point2   = NULL,
  };

  return __closest_pair_rec_kdtree(this, this, result, distance);
}

static void __generate_dot_rec_kdtree(
  KDTree _this, FILE *fp, char *(*to_string)(const Item item), int prof) {
  struct KDTree *this = (struct KDTree *) _this;

  char *strA = to_string(this->value);
  fprintf(fp, "\"%s\" [label=\"%d\n%s\"];\n", strA, prof % this->dim, strA);

  if (this->left) {
    char *strB = to_string(this->left->value);

    fprintf(fp, "\"%s\" -> \"%s\";\n", strA, strB);
    __generate_dot_rec_kdtree(this->left, fp, to_string, prof + 1);

    free(strB);
  } else {
    fprintf(fp, "\"n%s\" [label=\"nil\",style=invis];\n", strA);
    fprintf(fp, "\"%s\" -> \"n%s\"[style=invis];\n", strA, strA);
  }

  if (this->right) {
    char *strB = to_string(this->right->value);

    fprintf(fp, "\"%s\" -> \"%s\";\n", strA, strB);
    __generate_dot_rec_kdtree(this->right, fp, to_string, prof + 1);

    free(strB);
  } else {
    fprintf(fp, "\"n%s\" [label=\"nil\",style=invis];\n", strA);
    fprintf(fp, "\"%s\" -> \"n%s\"[style=invis];\n", strA, strA);
  }

  free(strA);
}

static void __generate_dot_kdtree(
  KDTree this, FILE *fp, char *(*to_string)(const Item item)) {
  fprintf(fp, "digraph T {\n");
  fprintf(fp, "node [fontname=\"Arial\"];\n");
  __generate_dot_rec_kdtree(this, fp, to_string, 0);
  fprintf(fp, "}\n");
}

static void __destroy_rec_kdtree(KDTree _this, void (*destroy)(Item item)) {
  struct KDTree *this = (struct KDTree *) _this;

  if (this->left)
    __destroy_rec_kdtree(this->left, destroy);

  if (this->right)
    __destroy_rec_kdtree(this->right, destroy);

  if (destroy && this->value)
    destroy(this->value);

  free(this);
}

static void __destroy_kdtree(KDTree _this, void (*destroy)(Item item)) {
  struct KDTree *this = (struct KDTree *) _this;

  free(this->funcs);

  __destroy_rec_kdtree(this, destroy);
}

const struct KDTree_t KDTree_t = {  //
  .create           = &__create_kdtree,
  .insert           = &__insert_kdtree,
  .delete           = &__delete_kdtree,
  .search           = &__search_kdtree,
  .get              = &__get_kdtree,
  .find_min         = &__find_min_kdtree,
  .is_empty         = &__is_empty_kdtree,
  .is_leaf          = &__is_leaf_kdtree,
  .passe_simetrico  = &__passe_simetrico_kdtree,
  .range_search     = &__range_search_kdtree,
  .nearest_neighbor = &__nearest_neighbor_kdtree,
  .closest_pair     = &__closest_pair_kdtree,
  .generate_dot     = &__generate_dot_kdtree,
  .destroy          = &__destroy_kdtree};