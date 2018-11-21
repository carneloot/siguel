// Entrada:   sufixo
/* Descrição: Detecta colisão. Trecho da rua onde ocorreu colisão fica interditado.
              Saída .txt: placa dos carros que colidiram 
              Nome-base-sufixo.svg */
#include <model/comando.r>
#include <control/controlador.r>

#include <model/veiculo.h>  
#include <model/modules/sorts.h>


#include <model/modules/lista.h>

int compare( Veiculo this, Veiculo other ){
  double this_x  = get_x_veiculo(this);
  double other_x = get_x_veiculo(this);

  if( this_x > other_x ){
    return 1;
  }
  return -1;
}


int comando_qry_dc( void* _this, void* _controlador ){
  struct Comando *this = _this;
  struct Controlador *controlador = _controlador;

  /* Ordenar a lista de carros
      -Método a definir

    detectar colisões de forma eficiente
    Guardar colisões em uma lista
    Adicionar colisões em drawables
      -Retângulos transparentes com borda pontilhada

    criar um svg com tudo
  */
  Veiculo* vetor_veiculos = Lista_t.to_array( controlador->veiculos );
  int tamanho = Lista_t.length( controlador->veiculos );

  // Limpar retornar todas as arestas às velocidades originais

  heap_sort(vetor_veiculos, tamanho, compare);

  // Percorrer o vetor comparando as sobreposições
  int i = 0;
  int j = 1;

  double this_x;
  double other_x;
  double this_width;

  double this_y;
  double other_y;
  double this_height;
  double other_height;

  double this_high;  // y + height
  double other_high; // y + height

  while (i < tamanho){
    this_x     = get_x_veiculo( vetor_veiculos[ i ]);
    other_x    = get_x_veiculo( vetor_veiculos[ j ]);
    this_width = get_width_veiculo( vetor_veiculos[ i ] );


    if(other_x < this_x + this_width ){
      // Sobrepõe em x, comparar o y
      this_y       = get_y_veiculo      ( vetor_veiculos[ i ]);
      this_height  = get_height_veiculo ( vetor_veiculos[ i ]);
      other_y      = get_y_veiculo      ( vetor_veiculos[ j ]);
      other_height = get_height_veiculo ( vetor_veiculos[ j ]);

      this_high    = this_y + this_height;
      other_high   = other_y + other_height;

      if( ( other_high > this_x && other_high < this_high) ||
          ( other_y > this_y && other_y < this_high ) ){
            
        // =============================== Sobreposições ===============================
        // Alterar aresta
        // Adicionar retangulo pontilhado à lista de drawables
        // 

        
      }
      // Se sobrepõe em x, o próximo pode sobrepor também
      // então avança-se só o j
      j++;

      // Se o j maior que todos, significa que é melhor incrementar o i
      if( j <= tamanho ){
        continue;
      }
    }

    // Se não sobrepõe em x, avançar o i e voltar o j
    i++;
    j = i + 1;

  }


  // Desenhar svg com os drawables
  // Limpar a lista de drawables

  
 return 1;
}