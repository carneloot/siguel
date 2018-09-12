#include <comando.r>
#include <controlador.r>

#include <pessoa.h>
#include <figura.h>
#include <endereco.h>
#include <modules/logger.h>

int __comando_qry_dm(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;
  char *cpf     = params[0];

  HashTable tabela = controlador->tabelas[CPF_X_PESSOA];

  if (!HashTable_t.exists(tabela, cpf)) {
    LOG_ERRO("\"%s\" nao foi encontrado na tabela.", cpf);
    return 0;
  }

  HashInfo info = HashTable_t.get(tabela, cpf);

  Pessoa pessoa = info.valor;

  char *info_pessoa = pessoa_get_info(pessoa);
  strcat(info_pessoa, "\n");

  Lista_t.insert(controlador->saida, info_pessoa);

  Ponto2D posicao = endereco_get_coordenada(
    pessoa_get_endereco(pessoa), controlador);

  // TODO: Adicionar cpf no local que a pessoa mora.
  Figura figura = cria_circulo(
    posicao.x, posicao.y, 20,
    "transparent", "green"
  );

  Lista_t.insert(controlador->saida_svg_qry, figura);

  return 1;
}