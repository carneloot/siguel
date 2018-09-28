#include <model/comando.r>
#include <control/controlador.r>

#include <stdlib.h>
#include <string.h>

#include <model/pessoa.h>
#include <model/desenhavel.h>
#include <model/endereco.h>
#include <model/utils.h>

#include "svg_linha.h"

int comando_qry_mud(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char *cpf = this->params[0];

  HashTable tabela_pessoa = controlador->tabelas[CPF_X_PESSOA];
  HashTable tabela_cep    = controlador->tabelas[CPF_X_CEP];

  if (!HashTable_t.exists(tabela_pessoa, cpf)) {
    char *saida = format_string("CPF \"%s\" nao encontrado.\n", cpf);
    Lista_t.insert(controlador->saida, saida);
    return 1;
  } 

  HashInfo hash_info = HashTable_t.get(tabela_pessoa, cpf);
  Pessoa pessoa = hash_info.valor;

  char *cep   = this->params[1];
  int face    = char_to_face(this->params[2][0]);
  int numero  = strtol(this->params[3], NULL, 10);
  char *compl = this->params[4];

  Ponto2D pos_antiga = endereco_get_coordenada(pessoa_get_endereco(pessoa), controlador);
  char *info_pessoa = pessoa_get_info(pessoa, controlador);

  HashTable_t.remove(tabela_cep, cpf);
  pessoa_set_endereco(pessoa, cep, face, numero, compl);

  HashInfo new_info = {
    .chave = pessoa_get_cpf(pessoa),
    .valor = pessoa_get_cep(pessoa)
  };
  HashTable_t.insert(tabela_cep, new_info);

  Ponto2D pos_atual = endereco_get_coordenada(pessoa_get_endereco(pessoa), controlador);
  char *info_endereco_atual = endereco_get_info(pessoa_get_endereco(pessoa));

  char *saida = format_string(
    "Mudanca de endereco:\n\t%s\n\tmudou para %s\n",
    info_pessoa, info_endereco_atual);
  Lista_t.insert(controlador->saida, saida);

  free(info_pessoa);
  free(info_endereco_atual);

  // Desenhar uma linha de pos_antiga ate pos_atual
  Seta pontos = cria_svg_pontos(pos_antiga, pos_atual, "green", 1, 0);

  Ponto2D new_max = Ponto2D_t.maximo(pos_antiga, pos_atual);
  controlador->max_qry = Ponto2D_t.maximo(controlador->max_qry, new_max);

  Lista_t.insert(controlador->saida_svg_qry, 
    cria_desenhavel(pontos, svg_pontos, free_svg_pontos)
  );
  
  return 1;
}