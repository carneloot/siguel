#include <model/comando.r>
#include <control/controlador.r>

#include <model/pessoa.h>
#include <model/modules/logger.h>
#include <stdlib.h>

/**
 * Comando:    @m?
 * Parametros: r cpf
 * Descricao:  Armazena no registrador r a posição geográfica da residência do morador de cpf cpf
 */

int comando_reg_m(void *_this, void *_controlador) {
  struct Comando *this = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  int numero_registrador = strtol(this->params[0] + 1, NULL, 10);
  char *cpf = this->params[1];

  Pessoa pessoa = HashTable_t.get(controlador->tabelas[CPF_X_PESSOA], cpf).valor;

  if (pessoa == NULL) {
    LOG_ERRO("Nao foi possivel encontrar a pessoa de CPF \"%s\".", cpf);
    return 1;
  }

  Endereco endereco_pessoa = pessoa_get_endereco(pessoa);

  Ponto2D coordenadas = endereco_get_coordenada(endereco_pessoa, controlador);

  controlador->registradores[numero_registrador] = coordenadas;

  return 1;
}