#include <comando.r>
#include <controlador.r>

#include <string.h>
#include <stdlib.h>

#include <pessoa.h>
#include <elemento.h>
#include <modules/logger.h>
#include <utils.h>
#include "../funcoes_checagem.h"

static int contemCep(void *item, void *other) {
  Pessoa pessoa   = item;
  char *cep = other;

  char *cep_pessoa = pessoa_get_cep(pessoa);

  if (!cep_pessoa)
    return 1;

  return strcmp(cep, cep_pessoa);
}

static Lista_t comando_m_all(Lista_t lista, char *cep) {
  Posic_t it = lt_get_first(lista);
  Lista_t saida = lt_create();

  it = lt_search(lista, it, cep, contemCep);
  while (it) {
    lt_insert(saida, lt_get(lista, it));

    it = lt_search(lista, lt_get_next(lista, it), cep, contemCep);
  }

  return saida;
}

static void __reportar_pessoas(Lista_t pessoas, struct Controlador *controlador) {
  Posic_t it = lt_get_first(pessoas);
  while (it) {
    Pessoa pessoa = lt_get(pessoas, it);

    char *info_pessoa = pessoa_get_info(pessoa, controlador);

    lt_insert(controlador->saida, format_string("\t%s\n", info_pessoa));

    free(info_pessoa);

    it = lt_get_next(pessoas, it);
  }
}

static Lista_t __quadras_dentro(KDTree_t arvore, Figura figura) {

  Ponto2D_t pA = get_pos(figura);

  Ponto2D_t pB = pA;
  pB.y += get_h(figura);
  pB.x += get_w(figura);

  Lista_t quadras = kdt_range_search(arvore, elemento_dentro_rect, &pA, &pB);

  Posic_t it = lt_get_first(quadras);
  Posic_t next_it;
  while (it) {
    next_it = lt_get_next(quadras, it);

    Elemento quadra = lt_get(quadras, it);
    Figura figura_elemento = get_figura_elemento(quadra);

    if (!dentro_figura(figura, figura_elemento))
      lt_remove(quadras, it);

    destruir_figura(figura_elemento);

    it = next_it;
  }

  return quadras;
}

/**
 * Comando: m?
 * Params:  cep
 * Moradores da quadra cujo cep é cep. Mostra
 * mensagem de erro se quadra não existir.
 * SAIDA: arquivo .txt => listar o nome, endereço,
 * celular e operadora dos moradores da quadra.
 */
int comando_qry_m(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;
  char *cep = params[0];

  if (!ht_exists(controlador->tabelas[CEP_X_QUADRA], cep)) {
    lt_insert(controlador->saida,
      format_string("A quadra \"%s\" nao foi encontrada.\n", cep));
    return 1;
  }

  Lista_t pessoas_cep = comando_m_all(controlador->pessoas, cep);

  if (lt_length(pessoas_cep) == 0) {
    lt_insert(controlador->saida, format_string(
      "Nao ha pessoas na quadra %s\n", cep));
    lt_destroy(pessoas_cep, NULL);
    return 1;
  }

  char *string_saida = format_string("Pessoas na quadra %s:\n", cep);
  lt_insert(controlador->saida, string_saida);

  __reportar_pessoas(pessoas_cep, controlador);

  lt_destroy(pessoas_cep, NULL);

  return 1;
}

/**
 * Comando: mr?
 * Params:   x y larg alt
 * Moradores das quadras que estão inteiramente
 * dentro da região determinada.
 * SAIDA: arquivo .txt => para cada quadra da
 * região, produzir uma saida similar a da consulta
 * m?.
 */
int comando_qry_mr(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;

  Ponto2D_t pos  = p2d_new(strtod(params[0], NULL), strtod(params[1], NULL));
  Ponto2D_t size = p2d_new(strtod(params[2], NULL), strtod(params[3], NULL));
  
  Figura figura = cria_retangulo(pos.x, pos.y, size.x, size.y, "pink", "pink");

  Lista_t quadras_dentro = __quadras_dentro(controlador->elementos[QUADRA], figura);

  destruir_figura(figura);

  if (lt_length(quadras_dentro) == 0) {
    lt_insert(controlador->saida,
      format_string("Nao foram encontradas quadras na area especificada.\n"));
    lt_destroy(quadras_dentro, NULL);
    return 1;
  }

  char *string_saida = format_string(
    "Pessoas nas quadras entre (%.0f,%.0f) com tamanho (%.0f,%.0f):\n", 
    pos.x, pos.y, size.x, size.y);
  lt_insert(controlador->saida, string_saida);

  Posic_t it = lt_get_first(quadras_dentro);
  while (it) {
    Elemento quadra = lt_get(quadras_dentro, it);

    char *cep = get_cep_elemento(quadra);

    Lista_t pessoas_cep = comando_m_all(controlador->pessoas, cep);
    
    __reportar_pessoas(pessoas_cep, controlador);

    lt_destroy(pessoas_cep, NULL);

    it = lt_get_next(quadras_dentro, it);
  }

  lt_destroy(quadras_dentro, NULL);

  return 1;
}