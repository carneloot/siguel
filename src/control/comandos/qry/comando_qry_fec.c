#include <model/comando.r>
#include <control/controlador.r>

#include <stdlib.h>

#include <model/comercio.h>
#include <model/utils.h>
#include <model/modules/logger.h>

int procurarComercio(void * const com1, const void *com2) {
  return !(com1 == com2);
}

int comando_qry_fec(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char *cnpj = this->params[0];
  HashTable tabela = controlador->tabelas[CNPJ_X_COMERCIO];

  if (!HashTable_t.exists(tabela, cnpj)) {
    Lista_t.insert(controlador->saida, 
      format_string("CNPJ \"%s\" nao encontrado.", cnpj));
    return 1;
  }

  HashInfo hash_info = HashTable_t.get(tabela, cnpj);
  Comercio comercio = hash_info.valor;

  Lista comercios = controlador->comercios;
  Posic posic   = Lista_t.get_first(comercios);
  posic         = Lista_t.search(comercios, posic, comercio, procurarComercio);

  if (!posic) {
    LOG_ERRO("Erro interno.");
    exit(EXIT_FAILURE);
  }

  HashTable_t.remove(tabela, cnpj);
  Lista_t.remove(comercios, posic);

  // Mostrar informacoes
  char *tipo_desc = HashTable_t.get(controlador->tabelas[TIPO_X_DESCRICAO], comercio_get_tipo(comercio)).valor;
  char *info_comercio = comercio_get_info(comercio, tipo_desc);
  char *rip_message = format_string(
    "Nota de fechamento de comercio:\n\t%s\n", info_comercio);

  Lista_t.insert(controlador->saida, rip_message);

  free(info_comercio);

  comercio_destruir(comercio);

  return 1;
}