#include <comando.r>
#include <controlador.r>

#include <comercio.h>
#include <string.h>
#include <utils.h>
#include <stdlib.h>

static int compararCEP(void * const comercio, const void *outro) {
  return strcmp(comercio_get_endereco(comercio)->cep, outro);
}

static int compararTIPO(void * const comercio, const void *outro) {
  return strcmp(comercio_get_tipo(comercio), outro);
}

int __comando_qry_tecq(void *_this, void *_controlador) {
  struct Comando     *this        = _this;
  struct Controlador *controlador = _controlador;

  char *cep = this->params[0];

  HashTable tabela = controlador->tabelas[CEP_X_QUADRA];

  if (!HashTable_t.exists(tabela, cep)) {
    Lista_t.insert(controlador->saida, 
      format_string("O CEP \"%s\" nao foi encontrado.\n", cep));
    return 1;
  }

  Lista_t.insert(controlador->saida, 
    format_string("Comercios na quadra %s:\n", cep));

  Lista comercios = controlador->comercios;

  char **tipos = calloc(HashTable_t.length(controlador->tabelas[TIPO_X_DESCRICAO]), sizeof(*tipos));

  // Peguei todos os tipos
  Posic it = Lista_t.get_first(comercios);
  it = Lista_t.search(comercios, it, cep, compararCEP);
  while (it) {
    Comercio comercio = Lista_t.get(comercios, it);
    char *tipo_atual = comercio_get_tipo(comercio);

    char **tipos_it = tipos;
    while (*tipos_it && strcmp(tipo_atual, *tipos_it++));
    *tipos_it = tipo_atual;

    it = Lista_t.search(comercios, Lista_t.get_next(comercios, it), cep, compararCEP);
  }

  // Passar printando os tipos
  char **tipos_it = tipos;
  while (*tipos_it) {
    char *tipo_desc = HashTable_t.get(controlador->tabelas[TIPO_X_DESCRICAO], *tipos_it).valor;

    Lista_t.insert(controlador->saida, 
      format_string("\t%s:\n", tipo_desc));

    Posic it = Lista_t.get_first(comercios);
    it = Lista_t.search(comercios, it, *tipos_it, compararTIPO);
    while (it) {
      Comercio comercio = Lista_t.get(comercios, it);

      char *info_comercio = comercio_get_info(comercio, tipo_desc);

      Lista_t.insert(controlador->saida, 
        format_string("\t\t%s\n", info_comercio));

      free(info_comercio);

      it = Lista_t.search(comercios, Lista_t.get_next(comercios, it), *tipos_it, compararTIPO);
    }

    tipos_it++;
  }

  free(tipos);

  return 1;
}