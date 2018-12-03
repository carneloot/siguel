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
#include <stdlib.h>

#include <comando.r>
#include <controlador.r>
#include <veiculo.h>
#include <modules/lista.h>

int comando_qry_au( void* _this, void* _controlador ){
  struct Comando *this = _this;
  struct Controlador *controlador = _controlador;

  Ponto2D_t pos = p2d_new( strtod(this->params[1], NULL), strtod(this->params[2], NULL) );

  double w = strtod(this->params[3], NULL);
  double h = strtod(this->params[4], NULL);
  char* placa = this->params[0];

  Veiculo veiculo = cria_veiculo( pos, w, h, placa );

  lt_insert(controlador->veiculos, veiculo);
  
  // TODO Escrever a placa do carro no svg

  return 1;
}


