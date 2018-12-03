#include <comando.r>
#include <controlador.r>

#include <stdlib.h>
#include <string.h>

#include <comercio.h>
#include <desenhavel.h>
#include <endereco.h>
#include <utils.h>

#include "svg_linha.h"

/**
 * Comando: mudec
 * Params:  cnpj cep face num
 * O estabelecimento identificado por cnpj muda-se
 * para novo endereço. Similar ao mud. Obs. traçar
 * linha com cor diferente da de mud.
 */
int comando_qry_mudec(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char *cnpj = this->params[0];

  HashTable_t tabela = controlador->tabelas[CNPJ_X_COMERCIO];

  if (!ht_exists(tabela, cnpj)) {
    char *saida = format_string(
      "O comercio com o CNPJ \"%s\" nao foi encontrado.\n", cnpj);
    lt_insert(controlador->saida, saida);
    return 1;
  } 

  Comercio comercio = ht_get(tabela, cnpj);

  char *cep   = this->params[1];
  int face    = char_to_face(this->params[2][0]);
  int numero  = strtol(this->params[3], NULL, 10);

  Ponto2D_t pos_antiga  = endereco_get_coordenada(comercio_get_endereco(comercio), controlador);
  char *tipo_desc = ht_get(controlador->tabelas[TIPO_X_DESCRICAO], comercio_get_tipo(comercio));
  char *info_comercio = comercio_get_info(
    comercio, tipo_desc);

  comercio_set_endereco(comercio, cep, face, numero);

  Ponto2D_t pos_atual = endereco_get_coordenada(comercio_get_endereco(comercio), controlador);
  char *info_endereco_atual = endereco_get_info(comercio_get_endereco(comercio));

  char *saida = format_string(
    "Mudanca de endereco:\n\t%s\n\tmudou para %s\n",
    info_comercio, info_endereco_atual);
  lt_insert(controlador->saida, saida);

  free(info_comercio);
  free(info_endereco_atual);

  // Desenhar uma linha de pos_antiga ate pos_atual
  Seta pontos = cria_svg_pontos(pos_antiga, pos_atual, "red", 1, 0);

  Ponto2D_t new_max = p2d_maximo(pos_antiga, pos_atual);
  controlador->max_qry = p2d_maximo(controlador->max_qry, new_max);

  lt_insert(controlador->saida_svg_qry, 
    cria_desenhavel(pontos, svg_pontos, free_svg_pontos)
  );
  
  return 1;
}