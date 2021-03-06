#include <comando.r>
#include <controlador.r>

#include <stdlib.h>

#include <model/trabalho/comercio.h>
#include <model/trabalho/endereco.h>

#include <utils/logger.h>


/**
 * Comando: e
 * Params:  cnpj codt cep face num nome
 * Insere um novo estabelecimento comercial
 * de um determinado tipo (codt), localizado
 * em um dado endereço (cep,face,num), que
 * possui um dado cnpj e tem um dado nome.
 */
int comando_ec_e(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;

  int num  = strtol(params[4], NULL, 10);  
  int face = char_to_face(params[3][0]);

  Comercio comercio = cria_comercio(
    params[0], params[1], params[2], face, num, params[5]);

  lt_insert(controlador->comercios, comercio);

  ht_insert(controlador->tabelas[CNPJ_X_COMERCIO], comercio_get_cnpj(comercio), comercio);

  return 1;
}