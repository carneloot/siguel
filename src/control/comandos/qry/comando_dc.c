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
#include <model/modules/grafod.h>
#include <model/mapa_viario/vertice.h>

#include <stdlib.h>
#include <float.h>

struct Colisao {
  ArestaInfo aresta_info;
  Figura figura;
};

static int compare_x( const void* _this, const void* _other ){

  Veiculo this = (Veiculo) _this;
  Veiculo other = (Veiculo) _other;

  double this_x  = get_x_veiculo(this);
  double other_x = get_x_veiculo(other);

  if( this_x > other_x ){
    return 1;
  }
  return -1;
}

static double __distancia_aresta_ponto(const Item _aresta_info, const Item _ponto, int dim) {
  ArestaInfo aresta_info = (ArestaInfo) _aresta_info;

  Ponto2D ponto        = * (Ponto2D *) _ponto;
  Ponto2D ponto_aresta = aresta_info->pos;

  switch (dim) {
    case 0:  return sqr(ponto.x - ponto_aresta.x);
    case 1:  return sqr(ponto.y - ponto_aresta.y);
    case -1: return Ponto2D_t.dist_squared(ponto, ponto_aresta);
  }

  return DBL_MAX;
}

static void limpar_colisoes(Lista colisoes) {
  // Limpar retornar todas as arestas às velocidades originais
  while ( Lista_t.length(colisoes) > 0 ) {
    struct Colisao* colisao = Lista_t.remove(
      colisoes, Lista_t.get_first(colisoes));

    set_aresta_valido(colisao->aresta_info);

    destruir_figura(colisao->figura);
    free(colisao);
  }
}

static void desenhar_colisoes(Lista colisoes, SVG svg_saida) {
  for (Posic it = Lista_t.get_first(colisoes); it != NULL; it = Lista_t.get_next(colisoes, it)) {
    struct Colisao *colisao = Lista_t.get(colisoes, it);

    ArestaInfo info_aresta = colisao->aresta_info;

    escreve_comentario(svg_saida,
      "COLISAO",
      colisao->aresta_info->origem,
      colisao->aresta_info->destino);

    desenha_figura(svg_saida, colisao->figura, 1, false);

    #ifdef DEBUG

    Figura fig_aresta = cria_circulo( info_aresta->pos.x, info_aresta->pos.y, 5, "cyan", "transparent" );
    desenha_figura( svg_saida, fig_aresta, 1, false );
    destruir_figura( fig_aresta );

    #endif

  }
}


bool aresta_corresponde_colisao(struct Controlador* controlador, ArestaInfo aresta, Figura fig_colisao ){
  
  VerticeInfo vertice_origem  = GrafoD_t.get_info_vertice( controlador->mapa_viario, aresta->origem );
  VerticeInfo vertice_destino = GrafoD_t.get_info_vertice( controlador->mapa_viario, aresta->destino );

  // Verificar se a reta da aresta cruza uma das retas dos lados dos retângulos
  // São verificados apenas dois lados do retângulo
  Ponto2D origem;
  Ponto2D destino;
  Ponto2D colisao;
  Ponto2D colisao_tamanho;
  origem.x          = vertice_origem->pos.x;
  origem.y          = vertice_origem->pos.y;
  destino.x         = vertice_destino->pos.x;
  // destino.y         = vertice_destino->pos.y;
  colisao.x         = get_x( fig_colisao );
  colisao.y         = get_y( fig_colisao );
  colisao_tamanho.x = get_w( fig_colisao );
  colisao_tamanho.y = get_h( fig_colisao );

  if( origem.x == destino.x ){
    // Aresta é vertical
    if( origem.x > colisao.x && origem.x < (colisao.x + colisao_tamanho.x) ){
      return true;
    }
    return false;

  }else{
    // Aresta é horizontal
    if( origem.y > colisao.y && origem.y < (colisao.y + colisao_tamanho.y) ){
      return true;
    }
    return false;

  }
  
}

static ArestaInfo pegar_aresta_correspondente(struct Controlador* this, Ponto2D colisao_centro, Figura fig_colisao ) {
  KDTree arestas = this->arestas_mapa_viario;
  ArestaInfo info_aresta = KDTree_t.nearest_neighbor(
    arestas,
    &colisao_centro,
    __distancia_aresta_ponto).point1;

  if( !aresta_corresponde_colisao( this, info_aresta, fig_colisao ) ){
        // Retirar essa aresta da arvore, pegar outra aresta mais prox e conferir
        KDTree_t.remove( this->arestas_mapa_viario, info_aresta );
        ArestaInfo info_nova_aresta = KDTree_t.nearest_neighbor(
          arestas,
          &colisao_centro,
        __distancia_aresta_ponto).point1;
        KDTree_t.insert( this->arestas_mapa_viario, info_aresta );

        if( aresta_corresponde_colisao( this, info_nova_aresta, fig_colisao ) ){
          // Como é um ponteiro, basta igualar
          info_aresta = info_nova_aresta;
        }

  }
  return info_aresta;
}

int comando_qry_dc( void* _this, void* _controlador ){
  struct Comando *this = _this;
  struct Controlador *controlador = _controlador;

  limpar_colisoes(controlador->colisoes);
  // Colisões removidas

  Veiculo* vetor_veiculos = Lista_t.to_array( controlador->veiculos );
  int tamanho = Lista_t.length( controlador->veiculos );
  heap_sort(vetor_veiculos, tamanho, compare_x);

  // Percorrer o vetor comparando as sobreposições
  int i = 0;
  int j = 1;

  while (i < (tamanho - 1)){
    double this_x     = get_x_veiculo( vetor_veiculos[ i ]);
    double other_x    = get_x_veiculo( vetor_veiculos[ j ]);
    double this_width = get_width_veiculo( vetor_veiculos[ i ] );


    if(other_x < this_x + this_width ){
      // Sobrepõe em x, comparar o y
      double this_y       = get_y_veiculo      ( vetor_veiculos[ i ]);
      double this_height  = get_height_veiculo ( vetor_veiculos[ i ]);
      double other_y      = get_y_veiculo      ( vetor_veiculos[ j ]);
      double other_height = get_height_veiculo ( vetor_veiculos[ j ]);

      double this_high    = this_y + this_height;
      double other_high   = other_y + other_height;

      if( ( other_high > this_y && other_high < this_high) ||
          ( other_y > this_y && other_y < this_high ) ){

        // =============================== Sobreposições ===============================
        // Alterar aresta
        // Adicionar retangulo pontilhado à lista de drawables
        Figura veiculo_this  = get_figura_veiculo( vetor_veiculos[i]);
        Figura veiculo_other = get_figura_veiculo( vetor_veiculos[j]);
        Figura fig_colisao   = get_rect_sobreposicao( veiculo_this, veiculo_other, "red");

        Ponto2D colisao_centro = get_centro_massa( fig_colisao );
        

        ArestaInfo info_aresta = pegar_aresta_correspondente(controlador, colisao_centro, fig_colisao);

        struct Colisao* this_colisao = malloc( sizeof(struct Colisao) );

        this_colisao->aresta_info = info_aresta;
        this_colisao->figura      = fig_colisao;

        set_aresta_invalido( info_aresta );

        Lista_t.insert( controlador->colisoes, this_colisao );
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

  // Desenhando no svg
  char *nome_qry = get_nome(controlador->extras[e_qry]);
  char* sufixo = this->params[0];
  char* path = format_string( "%s%s-%s-%s.svg",
    controlador->dir_saida,
    controlador->nome_base,
    nome_qry, sufixo);
  free(nome_qry);

  SVG svg_saida = cria_SVG( path, controlador->max_qry.x, controlador->max_qry.y );

  free(path);

  desenhar_elementos( controlador, svg_saida );

  desenhar_mapa_viario( controlador, svg_saida );

  desenhar_veiculos( controlador, svg_saida );

  desenhar_colisoes( controlador->colisoes, svg_saida );

  salva_SVG( svg_saida );

  destruir_SVG( svg_saida );

  free(vetor_veiculos);

  return 1;
}