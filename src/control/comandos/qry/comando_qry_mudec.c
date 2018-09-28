#include <model/comando.r>
#include <control/controlador.r>

#include <stdlib.h>
#include <string.h>

#include <model/comercio.h>
#include <model/desenhavel.h>
#include <model/endereco.h>
#include <model/utils.h>

#include "svg_linha.h"

int comando_qry_mudec(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char *cnpj = this->params[0];

  HashTable tabela = controlador->tabelas[CNPJ_X_COMERCIO];

  if (!HashTable_t.exists(tabela, cnpj)) {
    char *saida = format_string(
      "O comercio com o CNPJ \"%s\" nao foi encontrado.\n", cnpj);
    Lista_t.insert(controlador->saida, saida);
    return 1;
  } 

  HashInfo hash_info = HashTable_t.get(tabela, cnpj);
  Comercio comercio = hash_info.valor;

  char *cep   = this->params[1];
  int face    = char_to_face(this->params[2][0]);
  int numero  = strtol(this->params[3], NULL, 10);

  Ponto2D pos_antiga  = endereco_get_coordenada(comercio_get_endereco(comercio), controlador);
  char *tipo_desc = HashTable_t.get(controlador->tabelas[TIPO_X_DESCRICAO], comercio_get_tipo(comercio)).valor;
  char *info_comercio = comercio_get_info(
    comercio, tipo_desc);

  comercio_set_endereco(comercio, cep, face, numero);

  Ponto2D pos_atual = endereco_get_coordenada(comercio_get_endereco(comercio), controlador);
  char *info_endereco_atual = endereco_get_info(comercio_get_endereco(comercio));

  char *saida = format_string(
    "Mudanca de endereco:\n\t%s\n\tmudou para %s\n",
    info_comercio, info_endereco_atual);
  Lista_t.insert(controlador->saida, saida);

  free(info_comercio);
  free(info_endereco_atual);

  // Desenhar uma linha de pos_antiga ate pos_atual
  Seta pontos = cria_svg_pontos(pos_antiga, pos_atual, "red", 1, 0);

  Ponto2D new_max = Ponto2D_t.maximo(pos_antiga, pos_atual);
  controlador->max_qry = Ponto2D_t.maximo(controlador->max_qry, new_max);

  Lista_t.insert(controlador->saida_svg_qry, 
    cria_desenhavel(pontos, svg_pontos, free_svg_pontos)
  );
  
  return 1;
}