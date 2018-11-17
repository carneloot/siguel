#include <model/comando.r>
#include <control/controlador.r>

#include <stdlib.h>
#include <string.h>

#include <model/desenhavel.h>
#include <model/elemento.h>
#include <model/comercio.h>
#include <model/endereco.h>
#include <model/pessoa.h>
#include <model/figura.h>
#include <model/utils.h>
#include "svg_custom.h"
#include "../funcoes_checagem.h"

#define NUCLEAR_SVG \
"<svg version=\"1.1\" id=\"Capa_1\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" x=\"0px\" y=\"0px\"" \
"	viewBox=\"0 0 512 512\" style=\"opacity:0.8;enable-background:new 0 0 512 512;\" xml:space=\"preserve\">" \
"<polygon style=\"fill:#EB874B;\" points=\"512,245.556 371.069,208.064 442.546,82.405 310.181,143.136 283.136,1.508 221.273,132.046 " \
"	108.361,40.717 145.946,179.982 0,181.687 119.446,264.514 8.756,358.455 154.173,346.089 130.531,488.311 233.878,386.538 " \
"	308.347,510.492 321.266,366.933 459.001,414.621 375.446,296.448 \"/>" \
"<polygon style=\"fill:#FFC850;\" points=\"421.161,249.262 330.238,225.073 376.352,144.003 290.956,183.185 273.507,91.811 " \
"	233.595,176.029 160.749,117.108 184.997,206.956 90.839,208.056 167.901,261.493 96.488,322.1 190.305,314.122 175.052,405.878 " \
"	241.728,340.218 289.772,420.189 298.107,327.57 386.968,358.336 333.062,282.096 \"/>" \
"<polygon style=\"fill:#FFDC64;\" points=\"357.161,251.873 301.471,237.058 329.716,187.402 277.41,211.401 266.723,155.434 " \
"	242.277,207.018 197.659,170.929 212.511,225.961 154.839,226.634 202.039,259.364 158.299,296.486 215.762,291.6 206.42,347.8 " \
"	247.258,307.584 276.685,356.566 281.79,299.836 336.218,318.681 303.2,271.984 \"/>" \
"<polygon style=\"fill:#FFF082;\" points=\"296.258,254.358 274.095,248.462 285.336,228.701 264.52,238.251 260.267,215.979 " \
"	250.539,236.507 232.783,222.145 238.693,244.046 215.742,244.314 234.526,257.339 217.119,272.112 239.987,270.167 " \
"	236.269,292.533 252.521,276.528 264.232,296.021 266.264,273.445 287.923,280.944 274.784,262.361 \"/>" \
"</svg>"

static int __dentro_figura(void * const quadra, const void *figura) {
  const Figura fig_quadra = get_figura_elemento(quadra);

  int result = dentro_figura((Figura) figura, fig_quadra);

  destruir_figura(fig_quadra);

  return !result;
}

static int __comercioDentro(void * const comercio, const void *cep_quadra) {
  int result = strcmp(comercio_get_endereco(comercio)->cep, cep_quadra);

  if (result != 0)
    comercio_destruir(comercio);

  return result;
}

static Lista __get_elementos_dentro(KDTree arvore, Ponto2D pos, Ponto2D size, enum TipoElemento tipo) {
  Ponto2D pA = pos;
  Ponto2D pB = Ponto2D_t.add(pos, size);

  Lista saida = KDTree_t.range_search(arvore, elemento_dentro_rect, &pA, &pB);

  if (tipo == QUADRA) {
    Figura figura = cria_retangulo(pos.x, pos.y, size.x, size.y, "", "");

    Lista lista_aux = Lista_t.filter(saida, figura, __dentro_figura);

    Lista_t.destruir(saida, 0);

    saida = lista_aux;

    destruir_figura(figura);
  }
  

  if (Lista_t.length(saida) == 0) {
    Lista_t.destruir(saida, 0);
    return NULL;
  }

  return saida;
}

static void __adicionarHashComercio(void * const comercio, const void *tabela) {
  HashInfo info = {
    .chave = comercio_get_cnpj(comercio),
    .valor = comercio
  };

  HashTable_t.insert((HashTable) tabela, info);
}

static int __pessoaDentro(void * const pessoa, const void *cep_quadra) {
  if (!pessoa_get_cep(pessoa)) return 1;
  return strcmp((char *) pessoa_get_cep(pessoa), cep_quadra);
}

static void __removerEnderecoPessoa(void * const pessoa, const void *tabela) {
  pessoa_remover_endereco(pessoa);
  HashTable_t.remove((HashTable) tabela, pessoa_get_cpf(pessoa));
}


static void __remover_elementos(
  struct Comando *this,
  struct Controlador *controlador,
  Lista *elementos) {

  for (int h = 0; h < 4; h++) {
    if (!elementos[h]) continue;

    const char *tipo_elemento = get_tipo_string_elemento(h);

    Posic it = Lista_t.get_first(elementos[h]);
    while (it) {
      Elemento elemento = Lista_t.get(elementos[h], it);

      KDTree_t.remove(controlador->elementos[h], elemento);
      char *cep   = get_cep_elemento(elemento);
      char *saida = format_string("\t%s: %s deletado (a).\n", tipo_elemento, cep);
      Lista_t.insert(controlador->saida, saida);

      // destruir_elemento(elemento);

      it = Lista_t.get_next(elementos[h], it);
    }

    Lista_t.destruir(elementos[h], destruir_elemento);

  }

}

/**
 * Comando: dpr
 * Params:  x y larg alt
 * Desapropria região.
 * Apagar quadras, hidrantes, etc dentro da região;
 * etc A pessoa não morre, mas deixa de ser morador.
 * SAIDA: arquivo .txt => listar o que foi removido
 * arquivo .svg => elementos removidos não devem
 * aparecer.
 */
int comando_qry_dpr(void *_this, void *_controlador) {
  struct Comando     *this        = _this;
  struct Controlador *controlador = _controlador;

  Ponto2D pos  = Ponto2D_t.new(strtod(this->params[0], 0), strtod(this->params[1], 0));
  Ponto2D size = Ponto2D_t.new(strtod(this->params[2], 0), strtod(this->params[3], 0));

  Lista elementos_dentro[4];

  for (int i = 0; i < 4; i++) {
    KDTree arvore = controlador->elementos[i];
    elementos_dentro[i] = __get_elementos_dentro(arvore, pos, size, i);
  }

  Lista_t.insert(controlador->saida, 
    format_string("Comando DPR:\n"));


  if (elementos_dentro[QUADRA]) {

    Posic it = Lista_t.get_first(elementos_dentro[QUADRA]);
    while (it) {
      Elemento quadra = Lista_t.get(elementos_dentro[QUADRA], it);

      // Retirar das pessoas os endereços das quadras destruidas e remover da tabela CPF/CEP
      Lista pessoas_dentro = Lista_t.filter(controlador->pessoas, get_cep_elemento(quadra), __pessoaDentro);
      Lista_t.map(pessoas_dentro, controlador->tabelas[CPF_X_CEP], __removerEnderecoPessoa);
      Lista_t.destruir(pessoas_dentro, 0);

      // Destruir os comercios nas quadras e tirar da tabela CNPJ/COMERCIO
      Lista new_comercios = Lista_t.filter(controlador->comercios, get_cep_elemento(quadra), __comercioDentro);
      Lista_t.destruir(controlador->comercios, 0);
      HashTable_t.destroy(controlador->tabelas[CNPJ_X_COMERCIO], NULL, 0);

      controlador->comercios = new_comercios;
      controlador->tabelas[CNPJ_X_COMERCIO] = HashTable_t.create(Lista_t.length(new_comercios));

      Lista_t.map(new_comercios, controlador->tabelas[CNPJ_X_COMERCIO], __adicionarHashComercio);

      // Remover da tabela CEP/QUADRA as quadras removidas
      HashTable_t.remove(controlador->tabelas[CEP_X_QUADRA], get_cep_elemento(quadra));

      it = Lista_t.get_next(elementos_dentro[QUADRA], it);
    }
  }

  // Remover da tabela ID/RADIO_BASE as radios removidas
  if (elementos_dentro[RADIO_BASE]) {
    Posic it = Lista_t.get_first(elementos_dentro[RADIO_BASE]);
    while (it) {
      Elemento radio_base = Lista_t.get(elementos_dentro[RADIO_BASE], it);
      HashTable_t.remove(controlador->tabelas[ID_X_RADIO], get_id_elemento(radio_base));
      it = Lista_t.get_next(elementos_dentro[RADIO_BASE], it);
    }
  }

  // Remover todos os equipamentos em elementos_dentro
  __remover_elementos(this, controlador, elementos_dentro);
  
  double tamanho = min(size.x, size.y) * 0.8;

  Figura area_afetada = cria_retangulo(pos.x, pos.y, size.x, size.y, "transparent", "teal");
  set_dashed_figura(area_afetada, FIG_BORDA_TRACEJADA);
  
  Ponto2D posicao_asset = Ponto2D_t.new(0, 0);
  posicao_asset = Ponto2D_t.sub_scalar(posicao_asset, tamanho / 2);

  // posicao_asset agora marca o centro do local
  // Agora é só adicionar a posicao do centro do retangulo para posicao_asset
  posicao_asset = Ponto2D_t.add(posicao_asset, get_centro_massa(area_afetada));

  void *custom = cria_custom(posicao_asset, tamanho, NUCLEAR_SVG, NULL);

  Lista_t.insert(controlador->saida_svg_qry, cria_desenhavel(
    custom, print_custom_asset, free_custom));

  Lista_t.insert(controlador->saida_svg_qry, cria_desenhavel(
    area_afetada, get_svg_figura, destruir_figura));

  return 1;
}