#include <comando.r>
#include <controlador.r>

#include <string.h>

#include <elemento.h>
#include "../funcoes_checagem.h"

/**
 * Comando: crd?
 * Params:  ( cep | id )
 * Imprime no arquivo .txt as coordenadas e a
 * espécie do equipamento urbano de um
 * determinado cep ou com uma determinada
 * identificação. Atenção: quadras e
 * equipamentos que foram removidos por
 * comandos d* e D* devem efetivamente ter
 * sido removidos.
 */
int comando_qry_crd(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;

  char *cep = params[0];

  for (int i = 0; i < 4; i++) {
    KDTree arvore   = controlador->elementos[i];
    Elemento result = NULL;

    KDTree_t.passe_simetrico(arvore, compararCEP, cep, &result);

    if (!result)
      continue;

    char *saida = get_info_elemento(result);
    strcat(saida, "\n");
    lt_insert(controlador->saida, saida);
  }

  return 1;
}