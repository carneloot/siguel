// Entrada:     placa
/* Descrição:   Remove carro identificado por placa.
                Note que colisões que eventualmente foram desfeitas só serão detectadas na próxima execução do comando dc
                Saída  .txt: imprimir a placa do carro e a posição onde estava */


/*

  Procurar carro por placa
  Remover carro da lista

  Escrever a placa do carro e a posição onde estava no txt
*/

#include <comando.r>
#include <controlador.r>

#include <model/colecao/lista.h>

#include <model/trabalho/veiculo.h>

#include <utils/logger.h>


int comparar_placa( void* _veiculo, void* _placa ){
  Veiculo veiculo = _veiculo;
  char* placa = (char*) _placa;

  return strcmp( get_placa_veiculo( veiculo ), placa );
}

int comando_qry_rau( void* _this, void* _controlador ){
  struct Comando *this = _this;
  struct Controlador *controlador = _controlador;

  char* placa = this->params[0];

  Posic_t atual = lt_get_first(controlador->veiculos);

  // Encontrar veículo
  atual = lt_search( controlador->veiculos, atual, placa, comparar_placa );
  
  if( atual == NULL){
    LOG_ERRO(LOG_FILE, "Tendando remover um veiculo que nao existe");
    return 0;
  }

  // free em veículo
  Veiculo veiculo = lt_get( controlador->veiculos, atual );
  destruir_veiculo( veiculo );

  // Tirar veículo da lista
  lt_remove(controlador->veiculos, atual);

  return 1;
}
