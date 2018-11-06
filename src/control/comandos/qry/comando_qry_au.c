// Entrada:     placa x y larg alt

/* Descrição:   Carro posicionado em algum lugar da cidade. 
                As dimensões são representadas pelo retângulo do argumento.
                O carro é identificado por placa.
                Saída SVG: desenhar um retângulo representando o carro. Escrever a placa no interior desse retângulo */


/* 
  Malloc tamanho carro
  
  receber dados do carro


  Adicionar o carro no svg // Pode ser removido
  
  adicionar o carro à lista ligada

*/

#include <model/comando.r>
#include <control/controlador.r>

#include <model/veiculo.h>
#include <model/modules/lista.h>

int comando_qry_au( void* _this, void* _controlador ){
  struct Comando *this = _this;
  struct Controlador *controlador = _controlador;

  Veiculo veiculo = cria_veiculo( strtod(this->params[1]), strtod(this->params[2]), strtod(this->params[3]), strtod(this->params[4]), this->params[0] );

  Lista_t.insert(controlador->veiculos, veiculo);
  Lista_t.insert(controlador->figuras, get_figura_veiculo( veiculo ));
  // TODO Escrever a placa do carro no svg

  return 1;
}


