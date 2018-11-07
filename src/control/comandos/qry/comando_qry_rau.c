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

int comando_qry_rau( void* _this, void* _controlador ){
  struct Comando *this = _this;
  struct Controlador *controlador = _controlador;

  Posic atual = Lista_t.get_first(controlador->veiculos);

  // Desculpaaa, só vi que tinha search depois
  while( !strcmp( this->params[0], 
                  get_placa_veiculo( Lista_t.get(controlador->veiculos, atual))
                  ) == 0 )
  {
    atual = Lista_t.get_next;
  }

  if( atual == NULL){
    LOG_ERRO(LOG_FILE, "Tendando remover um veiculo que nao existe");
    return 0;
  }

  Lista_t.remove(controlador->veiculos, atual);

  Lista_t.remove(controlador->figuras, get_figura_veiculo(Lista_t.get(controlador->veiculos, atual)) );

  // TODO: Remover a placa do carro do SVG

  return 1;
}
