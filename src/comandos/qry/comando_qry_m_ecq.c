#include <comando.r>
#include <controlador.r>

#include <string.h>
#include <stdlib.h>
#include <comercio.h>
#include <utils.h>

static int contemCep(const Item item, const void *other) {
  Comercio comercio = item;
  const char *cep   = other;

  char *cep_comercio = comercio_get_endereco(comercio)->cep;

  if (!cep_comercio)
    return 1;

  return strcmp(cep, cep_comercio);
}
int __comando_qry_ecq(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char *cep = this->params[0];

  Lista comercios = controlador->comercios;

  Posic it = Lista_t.get_first(comercios);
  it = Lista_t.search(comercios, it, cep, contemCep);

  if (it)
    Lista_t.insert(controlador->saida, 
      format_string("Comercios na quadra %s.\n", cep));

  while (it) {
    Comercio comercio = Lista_t.get(comercios, it);

    char *tipo_desc = HashTable_t.get(controlador->tabelas[TIPO_X_DESCRICAO], comercio_get_tipo(comercio)).valor;
    char *info_comercio = comercio_get_info(comercio, tipo_desc);
    strcat(info_comercio, "\n");

    Lista_t.insert(controlador->saida, info_comercio);

    it = Lista_t.search(comercios, Lista_t.get_next(comercios, it), cep, contemCep);
  }

  return 1;
}