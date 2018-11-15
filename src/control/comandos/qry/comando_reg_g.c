#include <model/comando.r>
#include <control/controlador.r>

#include <model/elemento.h>
#include <model/modules/logger.h>
#include <stdlib.h>

/**
 * Comando:    @g?
 * Parametros: r id
 * Descricao:  Armazena no registrador r a posição geográfica
 *             do equipamento urbano cujo identificador é id
 */

int comando_reg_g(void *_this, void *_controlador) {
  struct Comando *this = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  int numero_registrador = strtol(this->params[0] + 1, NULL, 10);
  char *id_equipamento = this->params[1];

  Elemento encontrado = NULL;

  for (int i = ID_X_RADIO; i <= ID_X_SEMAFORO; i++) {
    if (HashTable_t.exists(controlador->tabelas[i], id_equipamento)) {
      encontrado = HashTable_t.get(controlador->tabelas[i], id_equipamento).valor;
      break;
    }
  }

  if (encontrado == NULL) {
    LOG_ERRO("Equipamento com id \"%d\" nao encontrado.", id_equipamento);
    return 0;
  }

  controlador->registradores[numero_registrador] = get_pos(encontrado);

  return 1;
}