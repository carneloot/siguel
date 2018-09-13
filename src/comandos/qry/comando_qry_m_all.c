#include <comando.r>
#include <controlador.r>

#include <string.h>
#include <stdlib.h>
#include <pessoa.h>
#include <elemento.h>
#include <modules/logger.h>
#include <utils.h>
#include "../funcoes_checagem.h"

int contemCep(const Item item, const void *other) {
  Pessoa pessoa   = item;
  const char *cep = other;

  char *cep_pessoa = pessoa_get_cep(pessoa);

  if (!cep_pessoa)
    return 1;

  return strcmp(cep, cep_pessoa);
}

static Lista __comando_m_all(Lista lista, char *cep) {
  Posic it = Lista_t.get_first(lista);
  Lista saida = Lista_t.create();

  it = Lista_t.search(lista, it, cep, contemCep);
  while (it) {
    Lista_t.insert(saida, Lista_t.get(lista, it));

    it = Lista_t.search(lista, Lista_t.get_next(lista, it), cep, contemCep);
  }

  return saida;
}

static void __reportar_pessoas(Lista pessoas, struct Controlador *controlador) {
  Posic it = Lista_t.get_first(pessoas);
  while (it) {
    Pessoa pessoa = Lista_t.get(pessoas, it);

    char *info_pessoa = pessoa_get_info(pessoa, controlador);
    strcat(info_pessoa, "\n");

    Lista_t.insert(controlador->saida, info_pessoa);

    it = Lista_t.get_next(pessoas, it);
  }
}

int __comando_qry_m(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;
  char *cep = params[0];

  if (!HashTable_t.exists(controlador->tabelas[CEP_X_QUADRA], cep)) {
    LOG_ERRO("A quadra \"%s\" nao foi encontrada.", cep);
    return 1;
  }

  char *string_saida = format_string("Pessoas na quadra %s:\n", cep);
  Lista_t.insert(controlador->saida, string_saida);

  Lista pessoas_cep = __comando_m_all(controlador->pessoas, cep);

  __reportar_pessoas(pessoas_cep, controlador);

  Lista_t.destruir(pessoas_cep, NULL);

  return 1;
}

static Lista __quadras_dentro(KDTree arvore, Figura figura) {

  Ponto2D pA = get_pos(figura);

  Ponto2D pB = pA;
  pB.y += get_h(figura);
  pB.x += get_w(figura);

  Lista quadras = KDTree_t.range_search(arvore, elemento_dentro_rect, &pA, &pB);

  Posic it = Lista_t.get_first(quadras);
  Posic next_it;
  while (it) {
    next_it = Lista_t.get_next(quadras, it);

    Elemento quadra = Lista_t.get(quadras, it);
    Figura figura_elemento = get_figura_elemento(quadra);

    if (!dentro_figura(figura, figura_elemento))
      Lista_t.remove(quadras, it);

    destruir_figura(figura_elemento);

    it = next_it;
  }

  return quadras;
}

int __comando_qry_mr(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;

  Ponto2D pos  = Ponto2D_t.new(strtod(params[0], NULL), strtod(params[1], NULL));
  Ponto2D size = Ponto2D_t.new(strtod(params[2], NULL), strtod(params[3], NULL));
  
  Figura figura = cria_retangulo(pos.x, pos.y, size.x, size.y, "pink", "pink");

  Lista quadras_dentro = __quadras_dentro(controlador->elementos[QUADRA], figura);

  destruir_figura(figura);

  if (Lista_t.length(quadras_dentro) == 0) {
    LOG_ERRO("Nao foram encontradas quadras na area especificada.");
    Lista_t.destruir(quadras_dentro, NULL);
    return 1;
  }

  char *string_saida = format_string(
    "Pessoas nas quadras entre (%.0f,%.0f) com tamanho (%.0f,%.0f):\n", 
    pos.x, pos.y, size.x, size.y);
  Lista_t.insert(controlador->saida, string_saida);

  Posic it = Lista_t.get_first(quadras_dentro);
  while (it) {
    Elemento quadra = Lista_t.get(quadras_dentro, it);

    char *cep = get_cep_elemento(quadra);

    Lista pessoas_cep = __comando_m_all(controlador->pessoas, cep);
    
    __reportar_pessoas(pessoas_cep, controlador);

    Lista_t.destruir(pessoas_cep, NULL);

    it = Lista_t.get_next(quadras_dentro, it);
  }

  Lista_t.destruir(quadras_dentro, NULL);

  return 1;
}