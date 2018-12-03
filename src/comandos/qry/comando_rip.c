#include <comando.r>
#include <controlador.r>

#include <stdlib.h>

#include <desenhavel.h>
#include <figura.h>
#include <pessoa.h>
#include <utils.h>
#include <modules/logger.h>

#include "svg_custom.h"

#define SVG_FIGURA_CRUZ "<svg style=\"isolation:isolate\" viewBox=\"505.672 93 291.328 291.328\"><g><rect x=\"548.336\" y=\"135.664\" width=\"206\" height=\"206\" transform=\"matrix(0.707,0.707,-0.707,0.707,359.533,-390.661)\" fill=\"rgb(46,46,46)\"/><path d=\" M 663.836 229.164 L 663.836 142.664 L 638.836 142.664 L 638.836 229.164 L 555.336 229.164 L 555.336 254.164 L 638.836 254.164 L 638.836 334.664 L 663.836 334.664 L 663.836 254.164 L 747.336 254.164 L 747.336 229.164 L 663.836 229.164 Z \" fill-rule=\"evenodd\" fill=\"rgb(235,235,235)\"/></g></svg>"
#define TAMANHO_CRUZ 20

int procurarPessoa(void * pessoa1, void *pessoa2) {
  return !(pessoa1 == pessoa2);
}

/**
 * Comando: rip
 * Params:  cpf
 * Infelizmente pessoa identificada por cpf morreu.
 * Todos os dados armazenados sobre ela devem ser
 * apagados (dados pessoais, dados sobre moradia,
 * etc).
 * SAIDA: arquivo .txt => imprimir os dados
 * removidos de cada uma das estruturas. Por
 * exemplo:
 * RIP: José Silva, portador CPF, do sexo
 * Masculino, nascido à 01/01/1920, residia no
 * endereço CEP/NUM/FACE.....
 * arquivo .svg: colocar um losango preto, com um
 * cruz branca no interior na residência do defunto
 */
int comando_qry_rip(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char *cpf = this->params[0];
  HashTable_t tabela = controlador->tabelas[CPF_X_PESSOA];

  if (!ht_exists(tabela, cpf)) {
    lt_insert(controlador->saida,
      format_string("CPF \"%s\" nao encontrado.\n", cpf));
    return 1;
  }

  Pessoa pessoa = ht_get(tabela, cpf);

  Lista_t pessoas = controlador->pessoas;
  Posic_t posic   = lt_get_first(pessoas);
  posic         = lt_search(pessoas, posic, pessoa, procurarPessoa);

  if (!posic) {
    LOG_ERRO("Erro interno.");
    return 0;
  }

  ht_remove(controlador->tabelas[CPF_X_PESSOA], cpf);
  ht_remove(controlador->tabelas[CPF_X_CEP],    cpf);
  lt_remove(pessoas, posic);

  // Mostrar informacoes
  Ponto2D_t posicao = endereco_get_coordenada(pessoa_get_endereco(pessoa), controlador);
  Ponto2D_t new_max = p2d_add_scalar(posicao, TAMANHO_CRUZ);
  posicao = p2d_sub_scalar(posicao, TAMANHO_CRUZ / 2);

  controlador->max_qry = p2d_maximo(controlador->max_qry, new_max);
  
  void *custom = cria_custom(posicao, TAMANHO_CRUZ, SVG_FIGURA_CRUZ, 0);
  lt_insert(controlador->saida_svg_qry, cria_desenhavel(
    custom, print_custom_asset, free_custom));


  char *info_pessoa = pessoa_get_info(pessoa, controlador);
  char *rip_message = format_string(
    "Nota de falecimento:\n\t%s\n", info_pessoa);

  lt_insert(controlador->saida, rip_message);

  free(info_pessoa);

  pessoa_destruir(pessoa);

  return 1;
}