#include <comando.r>
#include <controlador.r>

#include <stdlib.h>
#include <figura.h>
#include <pessoa.h>
#include <modules/logger.h>

#define SVG_FIGURA_CRUZ "<svg style=\"isolation:isolate\" viewBox=\"505.672 93 291.328 291.328\"><g><rect x=\"548.336\" y=\"135.664\" width=\"206\" height=\"206\" transform=\"matrix(0.707,0.707,-0.707,0.707,359.533,-390.661)\" fill=\"rgb(46,46,46)\"/><path d=\" M 663.836 229.164 L 663.836 142.664 L 638.836 142.664 L 638.836 229.164 L 555.336 229.164 L 555.336 254.164 L 638.836 254.164 L 638.836 334.664 L 663.836 334.664 L 663.836 254.164 L 747.336 254.164 L 747.336 229.164 L 663.836 229.164 Z \" fill-rule=\"evenodd\" fill=\"rgb(235,235,235)\"/></g></svg>"
#define TAMANHO_CRUZ 20

int procurarPessoa(void * const pessoa1, const void *pessoa2) {
  return !(pessoa1 == pessoa2);
}

int __comando_qry_rip(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char *cpf = this->params[0];
  HashTable tabela = controlador->tabelas[CPF_X_PESSOA];

  if (!HashTable_t.exists(tabela, cpf)) {
    LOG_ERRO("CPF \"%s\" nao encontrado.", cpf);
    return 1;
  }

  HashInfo hash_info = HashTable_t.get(tabela, cpf);
  Pessoa pessoa = hash_info.valor;

  Lista pessoas = controlador->pessoas;
  Posic posic   = Lista_t.get_first(pessoas);
  posic         = Lista_t.search(pessoas, posic, pessoa, procurarPessoa);

  if (!posic) {
    LOG_ERRO("Erro interno.");
    return 0;
  }

  HashTable_t.remove(controlador->tabelas[CPF_X_PESSOA], cpf);
  HashTable_t.remove(controlador->tabelas[CPF_X_CEP],    cpf);
  Lista_t.remove(pessoas, posic);

  // Mostrar informacoes
  Ponto2D posicao = endereco_get_coordenada(pessoa_get_endereco(pessoa), controlador);
  Ponto2D new_max = Ponto2D_t.add_scalar(posicao, TAMANHO_CRUZ);
  posicao = Ponto2D_t.sub_scalar(posicao, TAMANHO_CRUZ / 2);

  Figura cruz = cria_custom(posicao.x, posicao.y, TAMANHO_CRUZ, TAMANHO_CRUZ, SVG_FIGURA_CRUZ);
  controlador->max_qry = Ponto2D_t.maximo(controlador->max_qry, new_max);
  
  Lista_t.insert(controlador->saida_svg_qry, cruz);


  char *info_pessoa = pessoa_get_info(pessoa, controlador);
  char *rip_message = malloc(23 + strlen(info_pessoa));

  sprintf(rip_message, "Nota de falecimento:\n%s\n", info_pessoa);

  Lista_t.insert(controlador->saida, rip_message);

  free(info_pessoa);

  pessoa_destruir(pessoa);

  return 1;
}