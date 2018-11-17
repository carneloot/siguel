#include <model/comando.r>
#include <control/controlador.r>

#include <model/elemento.h>
#include "../funcoes_checagem.h"

/**
 * Comando: cc
 * Params:  ( cep | id ) cstrk cfill
 * Muda as cores do contorno e do
 * preenchimento da quadra identificada por
 * cep ou do equipamento urbano
 * identificado por id.
 * Saída: arquivo.svg: quadra ou equipamento
 * pintados com as novas cores.
 */
int comando_qry_cc(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;

  char *cep = params[0];

  char *cor_borda = params[1];
  char *cor       = params[2];

  for (int i = 0; i < 4; i++) {
    KDTree arvore   = controlador->elementos[i];
    Elemento result = NULL;

    KDTree_t.passe_simetrico(arvore, compararCEP, cep, &result);

    if (!result)
      continue;

    set_cor_elemento(result, cor);
    set_cor_borda_elemento(result, cor_borda);
  }

  return 1;
}