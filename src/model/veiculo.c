#include "veiculo.h"

#include <stdlib.h>
#include <string.h>

#include "figura.h"
#include "modules/ponto2d.h"

#define COR_VEICULO "lightgray"
#define COR_BORDA_VEICULO "dimgray"

struct Veiculo{
  Ponto2D pos;  // [   X    ,  Y    ]
  Ponto2D size; // [ width , height ]
  char* placa;
  Figura figura;
};

Veiculo cria_veiculo( double x, double y, double w, double h, char *placa ) {
  struct Veiculo* this =  malloc(sizeof(struct Veiculo));
  
  this->pos = Ponto2D_t.new(x, y);
  this->size = Ponto2D_t.new(w, h);
  
  this->placa = malloc( sizeof(char) * (strlen( placa ) + 1 ) );
  strcpy(this->placa, placa);

  this->figura = cria_retangulo(this->pos.x, this->pos.y, this->size.x, this->size.y, COR_VEICULO, COR_BORDA_VEICULO );
  return this;
}

bool colide_veiculo( Veiculo _this, Veiculo _other ){
  struct Veiculo* this = _this;
  struct Veiculo* other = _other;
  
  return sobrepoe_figura(this, other);
}

char* get_placa_veiculo( Veiculo _this ){
  struct Veiculo* this = _this;

  return this->placa;
}