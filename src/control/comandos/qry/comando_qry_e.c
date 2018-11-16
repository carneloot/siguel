// Comando @e
// Entrada: r cep face num
/* Armazena em r a posição geográfica do endereço
   Cep face num*/

/*
  Criar endereço
  Pegar coordenadas do endereço
  Guardar no registrador
*/

#include <model/comando.r>
#include <control/controlador.r>

#include <string.h>
#include <stdlib.h>

#include <model/endereco.h>
#include <model/modules/ponto2d.h>


int comando_qry_e(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;

  int  numero_registrador = strtol( params[0] + 1, NULL, 10 );

  char *cep          = params[1];
  char *char_face    = params[2];
  int  numero        = strtol( params[3], NULL, 10 );
  int  face;

  switch( char_face[0] ){
    case 'N': face = FACE_NORTE; break;
    case 'S': face = FACE_SUL;   break;
    case 'L': face = FACE_LESTE; break;
    case 'O': face = FACE_OESTE; break;
  }

  Endereco endereco;
  endereco = cria_endereco( cep, face, numero);
  Ponto2D ponto = endereco_get_coordenada(endereco, controlador);

  controlador->registradores[numero_registrador].x = ponto.x;
  controlador->registradores[numero_registrador].y = ponto.y;

  endereco_destruir(endereco);

  return 1;
}