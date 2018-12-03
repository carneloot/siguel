#include <model/comando.r>
#include <control/controlador.r>

#include <stdlib.h>
#include <string.h>

#include <model/desenhavel.h>
#include <model/pessoa.h>
#include <model/figura.h>
#include <model/endereco.h>
#include <model/modules/logger.h>
#include <model/utils.h>
#include "svg_custom.h"

/**
 * Comando: dm?
 * Params:   cpf
 * Imprime todos os dados do morador identificado
 * pelo cpf.
 * SAIDA: arquivo .txt => dados pessoais, seu
 * endereço, o número de seu celular (se houver) e
 * respectiva operadora, e coordenada geográfica de
 * onde mora.
 * arquivo .svg => colocar um circulo (ou retângulo,
 * ou outra forma geométrica) no lugar onde mora
 * com seu cpf
 */
int comando_qry_dm(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;
  char *cpf     = params[0];

  HashTable tabela = controlador->tabelas[CPF_X_PESSOA];

  if (!HashTable_t.exists(tabela, cpf)) {
    char *saida = format_string("CPF \"%s\" nao encontrado.\n", cpf);
    Lista_t.insert(controlador->saida, saida);
    return 1;
  }

  Pessoa pessoa = HashTable_t.get(tabela, cpf);


  char *info_pessoa = pessoa_get_info(pessoa, controlador);
  strcat(info_pessoa, "\n");

  Lista_t.insert(controlador->saida, info_pessoa);

  Ponto2D posicao = endereco_get_coordenada(
    pessoa_get_endereco(pessoa), controlador);

  Figura figura = cria_circulo(
    posicao.x, posicao.y, 20,
    "transparent", "green"
  );
  set_opacity_figura(figura, 0.8);
  set_dashed_figura(figura, FIG_BORDA_TRACEJADA);

  Lista_t.insert(controlador->saida_svg_qry,
    cria_desenhavel(figura, get_svg_figura, destruir_figura));

  posicao.y += 30;
  posicao.x -= strlen(cpf) * 3.0;
  void *custom = cria_custom(posicao, 12, cpf, "green");
  Lista_t.insert(controlador->saida_svg_qry,
    cria_desenhavel(custom, print_custom_texto, free_custom));

  return 1;
}