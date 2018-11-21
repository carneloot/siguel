#include "veiculo.h"

#include <stdlib.h>
#include <string.h>

#include "figura.h"
#include <model/modules/ponto2d.h>
#include <model/utils.h>

#define COR_VEICULO "lightgray"
#define COR_BORDA_VEICULO "dimgray"

struct Veiculo{
  Ponto2D pos;  // [   X    ,  Y    ]
  Ponto2D size; // [ width , height ]
  char* placa;
  Figura figura;
};

Veiculo cria_veiculo( Ponto2D pos, double w, double h, char *placa ) {
  struct Veiculo* this =  malloc(sizeof(struct Veiculo));

  this->pos = pos;
  this->size = Ponto2D_t.new(w, h);
  
  this->placa = calloc(  strlen( placa ) + 1, sizeof(char) );
  strcpy(this->placa, placa);

  this->figura = cria_retangulo(this->pos.x, this->pos.y, this->size.x, this->size.y, COR_VEICULO, COR_BORDA_VEICULO );
  set_stroke_size_figura(this->figura, 1.0);
  return this;
}

bool colide_veiculo( Veiculo _this, Veiculo _other ){
  struct Veiculo* this = _this;
  struct Veiculo* other = _other;
  
  return sobrepoe_figura(this, other);
}

void destruir_veiculo( Veiculo _this ){
  struct Veiculo* this = _this;
  free(this->placa);
  destruir_figura(this->figura);
  free(this);
}

bool igual_veiculo(Veiculo _this, Veiculo _other){
  struct Veiculo* this = _this;
  struct Veiculo* other = _other;

  if( !strcmp(this->placa, other->placa) == 0)
    return false;

  return true;
}

char* get_placa_veiculo( Veiculo _this ){
  struct Veiculo* this = _this;

  return this->placa;
}

char* get_svg_veiculo( Veiculo _this ){
  struct Veiculo* this = _this;

  bool vertical = false;

  char* svg_figura = get_svg_figura( this->figura );
  char* svg_placa;

  vertical = (this->size.x < this->size.y);

  if (vertical) {
    svg_placa = format_string( 
      "<text writing-mode=\"tb-rl\" x=\"%.1f\" y=\"%.1f\" "
      "style=\"fill:%s;font-size:%.1fpx;font-family:sans-serif\">%s</text>\n", 
      this->pos.x + 2, this->pos.y + 1, COR_BORDA_VEICULO, 2.0, this->placa 
    );
  } else {
    svg_placa = format_string( 
      "<text x=\"%.1f\" y=\"%.1f\" "
      "style=\"fill:%s;font-size:%.1fpx;font-family:sans-serif\">%s</text>\n", 
      this->pos.x + 1, this->pos.y + this->size.y - 1.5,
      COR_BORDA_VEICULO, 2.0, this->placa 
    );
  }

  char *svg_veiculo = format_string( "%s%s", svg_figura, svg_placa );

  free(svg_figura);
  free(svg_placa);

  return svg_veiculo;
}

double get_x_veiculo( Veiculo _this ){
  struct Veiculo* this = _this;
  return this->pos.x;
}

double get_y_veiculo( Veiculo _this ){
  struct Veiculo* this = _this;
  return this->pos.y;
}

double get_width_veiculo( Veiculo _this ){
  struct Veiculo* this = _this;
  return this->size.x;
}

double get_height_veiculo( Veiculo _this ){
  struct Veiculo* this = _this;
  return this->size.y;
}