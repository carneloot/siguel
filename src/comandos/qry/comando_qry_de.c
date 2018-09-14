#include <comando.r>
#include <controlador.r>

#include <comercio.h>
#include <desenhavel.h>
#include <figura.h>
#include <endereco.h>
#include <utils.h>
#include <modules/logger.h>
#include <stdlib.h>

#include "svg_custom.h"

int __comando_qry_de(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;
  char *cnpj    = params[0];

  HashTable tabela = controlador->tabelas[CNPJ_X_COMERCIO];

  if (!HashTable_t.exists(tabela, cnpj)) {
    Lista_t.insert(controlador->saida, 
      format_string("Nao foi encontrado o comercio de CNPJ %s.", cnpj));
    return 1;
  }

  HashInfo info = HashTable_t.get(tabela, cnpj);

  Comercio comercio = info.valor;

  char *tipo_desc = HashTable_t.get(controlador->tabelas[TIPO_X_DESCRICAO], comercio_get_tipo(comercio)).valor;

  char *info_comercio = comercio_get_info(comercio, tipo_desc);
  strcat(info_comercio, "\n");

  Lista_t.insert(controlador->saida, info_comercio);

  Ponto2D posicao = endereco_get_coordenada(
    comercio_get_endereco(comercio), controlador);

  Figura figura = cria_circulo(
    posicao.x, posicao.y, 20,
    "transparent", "red"
  );
  set_opacity_figura(figura, 0.8);
  set_dashed_figura(figura, FIG_BORDA_TRACEJADA);

  Lista_t.insert(controlador->saida_svg_qry,
    cria_desenhavel(figura, get_svg_figura, destruir_figura));

  posicao.y += 30;
  posicao.x -= strlen(cnpj) * 3.0;
  void *custom = cria_custom(posicao, 12, cnpj, "red");
  Lista_t.insert(controlador->saida_svg_qry,
    cria_desenhavel(custom, print_custom_texto, free_custom));

  return 1;
}