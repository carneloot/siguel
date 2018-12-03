#include <comando.r>
#include <controlador.r>

#include <stdlib.h>

#include <comercio.h>
#include <utils.h>
#include <modules/logger.h>

int procurarComercio(void * com1, void *com2) {
  return !(com1 == com2);
}

/**
 * Comando: fec
 * Params:  cnpj
 * O estabelecimento comercial identificado por cnpj
 * fechou. Os dados a cerca dele devem ser
 * removidos.
 * SAIDA: arquivo .txt. Mostrar os dados dos
 * estabelecimento fechado
 */
int comando_qry_fec(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char *cnpj = this->params[0];
  HashTable tabela = controlador->tabelas[CNPJ_X_COMERCIO];

  if (!HashTable_t.exists(tabela, cnpj)) {
    lt_insert(controlador->saida, 
      format_string("CNPJ \"%s\" nao encontrado.", cnpj));
    return 1;
  }

  Comercio comercio = HashTable_t.get(tabela, cnpj);

  Lista_t comercios = controlador->comercios;
  Posic_t posic   = lt_get_first(comercios);
  posic         = lt_search(comercios, posic, comercio, procurarComercio);

  if (!posic) {
    LOG_ERRO("Erro interno.");
    exit(EXIT_FAILURE);
  }

  HashTable_t.remove(tabela, cnpj);
  lt_remove(comercios, posic);

  // Mostrar informacoes
  char *tipo_desc = HashTable_t.get(controlador->tabelas[TIPO_X_DESCRICAO], comercio_get_tipo(comercio));
  char *info_comercio = comercio_get_info(comercio, tipo_desc);
  char *rip_message = format_string(
    "Nota de fechamento de comercio:\n\t%s\n", info_comercio);

  lt_insert(controlador->saida, rip_message);

  free(info_comercio);

  comercio_destruir(comercio);

  return 1;
}