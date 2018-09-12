#include <comando.r>
#include <controlador.r>

#include <comercio.h>
#include <figura.h>
#include <endereco.h>
#include <modules/logger.h>

int __comando_qry_de(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;
  char *cnpj    = params[0];

  HashTable tabela = controlador->tabelas[CNPJ_X_COMERCIO];

  if (!HashTable_t.exists(tabela, cnpj)) {
    LOG_ERRO("\"%s\" nao foi encontrado na tabela.", cnpj);
    return 0;
  }

  HashInfo info = HashTable_t.get(tabela, cnpj);

  Comercio comercio = info.valor;

  char *tipo_desc = HashTable_t.get(controlador->tabelas[TIPO_X_DESCRICAO], comercio_get_tipo(comercio)).valor;

  char *info_comercio = comercio_get_info(comercio, tipo_desc);
  strcat(info_comercio, "\n");

  Lista_t.insert(controlador->saida, info_comercio);

  Ponto2D posicao = endereco_get_coordenada(
    comercio_get_endereco(comercio), controlador);

  // TODO: Adicionar cnpj no local que fica o comercio.
  Figura figura = cria_circulo(
    posicao.x, posicao.y, 20,
    "transparent", "red"
  );

  Lista_t.insert(controlador->saida_svg_qry, figura);

  return 1;
}