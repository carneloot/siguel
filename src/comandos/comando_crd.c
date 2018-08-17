#include "../comando.r"
#include "../controlador.r"

#include <elemento.h>
#include <string.h>
#include "funcoes_checagem.h"

int __comando_crd(void *_this, void *_controlador) {
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
    Lista_t.insert(controlador->saida, saida);
  }

  return 1;
}