// Entrada:     placa
/* Descrição:   Remove carro identificado por placa.
                Note que colisões que eventualmente foram desfeitas só serão detectadas na próxima execução do comando dc
                Saída  .txt: imprimir a placa do carro e a posição onde estava */


/*

  Procurar carro por placa
  Remover carro da lista

  Escrever a placa do carro e a posição onde estava no txt
*/

#include <model/comando.r>
#include <control/controlador.r>

#include <model/veiculo.h>
#include <model/modules/lista.h>
#include <model/modules/logger.h>


int comparar_placa( const Item _veiculo, void* _placa ){
  Veiculo veiculo = _veiculo;
  char* placa = _placa;

  return strcmp( get_placa_veiculo( veiculo ), placa );
}

int comando_qry_rau( void* _this, void* _controlador ){
  struct Comando *this = _this;
  struct Controlador *controlador = _controlador;

  char* placa = this->params[0];

  Posic atual = Lista_t.get_first(controlador->veiculos);

  atual = Lista_t.search( controlador->veiculos, atual, placa, comparar_placa );
  
  if( atual == NULL){
    LOG_ERRO(LOG_FILE, "Tendando remover um veiculo que nao existe");
    return 0;
  }

  Lista_t.remove(controlador->veiculos, atual);

  Lista_t.remove(controlador->figuras, get_figura_veiculo(Lista_t.get(controlador->veiculos, atual)) );

  // TODO: Remover a placa do carro do SVG

  return 1;
}
