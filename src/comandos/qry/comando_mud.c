#include <comando.r>
#include <controlador.r>

#include <stdlib.h>
#include <string.h>

#include <pessoa.h>
#include <desenhavel.h>
#include <endereco.h>
#include <utils.h>

#include "svg_linha.h"

/**
 * Comando: mud
 * Params:  cpf cep face num compl
 * A pessoa identificada por cpf muda-se para o
 * endereço determinado pelos parâmetros.
 * SAIDAS: arquivo .txt. Mostrar os dados da pessoa
 * (nome, etc), o endereço antigo e o novo endereço.
 * arquivo. svg: traça uma seta (linha grossa) da
 * endereço antigo ao endereço novo.
 */
int comando_qry_mud(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char *cpf = this->params[0];

  HashTable tabela_pessoa = controlador->tabelas[CPF_X_PESSOA];
  HashTable tabela_cep    = controlador->tabelas[CPF_X_CEP];

  if (!HashTable_t.exists(tabela_pessoa, cpf)) {
    char *saida = format_string("CPF \"%s\" nao encontrado.\n", cpf);
    lt_insert(controlador->saida, saida);
    return 1;
  } 

  Pessoa pessoa = HashTable_t.get(tabela_pessoa, cpf);

  char *cep   = this->params[1];
  int face    = char_to_face(this->params[2][0]);
  int numero  = strtol(this->params[3], NULL, 10);
  char *compl = this->params[4];

  Ponto2D_t pos_antiga = endereco_get_coordenada(pessoa_get_endereco(pessoa), controlador);
  char *info_pessoa = pessoa_get_info(pessoa, controlador);

  HashTable_t.remove(tabela_cep, cpf);
  pessoa_set_endereco(pessoa, cep, face, numero, compl);

  HashTable_t.insert(tabela_cep, pessoa_get_cpf(pessoa), pessoa_get_cep(pessoa));

  Ponto2D_t pos_atual = endereco_get_coordenada(pessoa_get_endereco(pessoa), controlador);
  char *info_endereco_atual = endereco_get_info(pessoa_get_endereco(pessoa));

  char *saida = format_string(
    "Mudanca de endereco:\n\t%s\n\tmudou para %s\n",
    info_pessoa, info_endereco_atual);
  lt_insert(controlador->saida, saida);

  free(info_pessoa);
  free(info_endereco_atual);

  // Desenhar uma linha de pos_antiga ate pos_atual
  Seta pontos = cria_svg_pontos(pos_antiga, pos_atual, "green", 1, 0);

  Ponto2D_t new_max = p2d_maximo(pos_antiga, pos_atual);
  controlador->max_qry = p2d_maximo(controlador->max_qry, new_max);

  lt_insert(controlador->saida_svg_qry, 
    cria_desenhavel(pontos, svg_pontos, free_svg_pontos)
  );
  
  return 1;
}