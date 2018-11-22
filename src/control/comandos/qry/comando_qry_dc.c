// Entrada:   sufixo
/* Descrição: Detecta colisão. Trecho da rua onde ocorreu colisão fica interditado.
              Saída .txt: placa dos carros que colidiram 
              Nome-base-sufixo.svg */
#include <model/comando.r>
#include <control/controlador.r>

#include <model/veiculo.h>  
#include <model/modules/sorts.h>
#include <model/modules/lista.h>
#include <model/mapa_viario/aresta.h>
#include <model/SVG.h>
#include <model/utils.h>

#include <stdlib.h>
#include <float.h>

struct Colisao {
  ArestaInfo aresta_info;

  double comprimento_backup;
  double velocidade_media_backup;
};

int compare( const void* _this, const void* _other ){

  Veiculo this = (Veiculo) _this;
  Veiculo other = (Veiculo) _other;

  double this_x  = get_x_veiculo(this);
  double other_x = get_x_veiculo(other);

  if( this_x > other_x ){
    return 1;
  }
  return -1;
}

static double __distancia_vertice_ponto(const Item _aresta_info, const Item _ponto, int dim) {
  ArestaInfo aresta_info = (ArestaInfo) _aresta_info;

  Ponto2D ponto         = * (Ponto2D *) _ponto;
  Ponto2D ponto_aresta = aresta_info->pos;

  switch (dim) {
    case 0:  return sqr(ponto.x - ponto_aresta.x);
    case 1:  return sqr(ponto.y - ponto_aresta.y);
    case -1: return Ponto2D_t.dist_squared(ponto, ponto_aresta);
  }

  return DBL_MAX;
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
  
  while ( Lista_t.length(controlador->colisoes) > 0 ) {
    struct Colisao* colisao = Lista_t.remove( 
      controlador->colisoes,
      Lista_t.get_first(controlador->colisoes)
    );

    set_aresta_valido( 
      colisao->aresta_info,
      colisao->velocidade_media_backup,
      colisao->comprimento_backup
    );
    free(colisao);
  }

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

  char *nome_qry = get_nome(controlador->extras[e_qry]);
  char* sufixo = this->params[0];
  char* path = format_string( "%s%s-%s-%s.svg",
    controlador->dir_saida,
    controlador->nome_base,
    nome_qry,
    sufixo
    );
  
  free(nome_qry);

  SVG svg_saida = cria_SVG( path, controlador->max_qry.x, controlador->max_qry.y );
  desenhar_elementos( controlador, svg_saida );
  desenhar_mapa_viario( controlador, svg_saida );
  desenhar_veiculos( controlador, svg_saida );

  free(path);

  while (i < (tamanho - 1)){
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

      if( ( other_high > this_y && other_high < this_high) ||
          ( other_y > this_y && other_y < this_high ) ){

        // =============================== Sobreposições ===============================
        // Alterar aresta
        // Adicionar retangulo pontilhado à lista de drawables
        Figura veiculo_this = get_figura_veiculo( vetor_veiculos[i]);
        Figura veiculo_other = get_figura_veiculo( vetor_veiculos[j]);
        Figura fig_colisao = get_rect_sobreposicao( veiculo_this, veiculo_other, "red");

        Ponto2D colisao_centro = get_centro_massa( fig_colisao );

        Pair par_mais_proximo = KDTree_t.nearest_neighbor( controlador->arestas_mapa_viario, &colisao_centro, __distancia_vertice_ponto );
        ArestaInfo info_aresta = par_mais_proximo.point1;

        struct Colisao* this_colisao = malloc( sizeof(struct Colisao) );

        this_colisao->aresta_info             = info_aresta;
        this_colisao->comprimento_backup      = info_aresta->comprimento;
        this_colisao->velocidade_media_backup = info_aresta->velocidade_media;

        Lista_t.insert( controlador->colisoes, this_colisao );
        set_aresta_invalido( info_aresta );

        desenha_figura( svg_saida, fig_colisao, 1, false );
        destruir_figura(fig_colisao);
      }

      // Se sobrepõe em x, o próximo pode sobrepor também
      // então avança-se só o j
      j++;

      // Se o j maior que todos, significa que é melhor incrementar o i
      if( j < tamanho ){
        continue;
      }
    }

    // Se não sobrepõe em x, avançar o i e voltar o j
    i++;
    j = i + 1;
  }


  salva_SVG( svg_saida );
  destruir_SVG( svg_saida );
  return 1;
}