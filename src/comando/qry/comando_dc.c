// Entrada:   sufixo
/* Descrição: Detecta colisão. Trecho da rua onde ocorreu colisão fica interditado.
              Saída .txt: placa dos carros que colidiram 
              Nome-base-sufixo.svg */
#include <comando.r>
#include <controlador.r>

#include <stdlib.h>
#include <float.h>

#include <algoritmo/sorts.h>

#include <model/estrutural/SVG.h>

#include <model/colecao/lista.h>
#include <model/colecao/grafod.h>

#include <model/trabalho/veiculo.h>

#include <model/trabalho/mapa_viario/aresta.h>
#include <model/trabalho/mapa_viario/vertice.h>
#include <model/trabalho/mapa_viario/colisao.h>

#include <utils/utils.h>


#define between(valor, min, max) ( ((valor) < (min)) ? false : ((valor) > (max)) ? false : true )


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

static double __distancia_quadra_ponto(void *_quadra, void *_ponto, int dim) {
  Elemento quadra = (Elemento) _quadra;
  Ponto2D_t ponto_quadra = get_pos( quadra );
  Ponto2D_t ponto = * (Ponto2D_t *) _ponto;

  switch(dim){
    case  0: return sqr(ponto.x - ponto_quadra.x);
    case  1: return sqr(ponto.y - ponto_quadra.y);
    case -1: return p2d_dist_squared( ponto, ponto_quadra );
  }

  return DBL_MAX;
}

static int __aresta_dentro(void *value, int dim, void *_ponto_a, void *_ponto_b ){
  ArestaInfo aresta = (ArestaInfo) value;
  Ponto2D_t ponto_a = * (Ponto2D_t *) _ponto_a;
  Ponto2D_t ponto_b = * (Ponto2D_t *) _ponto_b;
  Ponto2D_t pos_aresta = aresta->pos;
  
  switch(dim){
    case 0:
      if (pos_aresta.x < ponto_a.x) return -1;
      if (pos_aresta.x > ponto_b.x) return 1;
      return 0;

    case 1:
      if (pos_aresta.y < ponto_a.y) return -1;
      if (pos_aresta.y > ponto_b.y) return 1;
      return 0;

    case -1:
      // Se estiver dentro retorna 1
      if( between( pos_aresta.x, ponto_a.x, ponto_b.x ) &&
          between( pos_aresta.y, ponto_a.y, ponto_b.y ) )
        return 1;
      return 0;
  }
  return -1;
}

static void limpar_colisoes(Lista_t colisoes) {
  // Limpar retornar todas as arestas às velocidades originais
  while ( lt_length(colisoes) > 0 ) {
    Colisao colisao = lt_remove(
      colisoes, lt_get_first(colisoes));

    //Arrumar cada aresta da lista de colisões
    Lista_t arestas_colisao = colisao_get_arestas( colisao );
    while( lt_length( arestas_colisao ) > 0 ){
      ArestaInfo aresta = lt_remove( arestas_colisao, lt_get_first(arestas_colisao) );
      set_aresta_valido(aresta);
    }
    colisao_destroy( colisao );
  }
}

static void set_arestas_invalido( Lista_t arestas ){

  for (Posic_t it = lt_get_first(arestas); it != NULL; it = lt_get_next(arestas, it) ) {
    ArestaInfo aresta = lt_get( arestas, it );
    set_aresta_invalido(aresta);
  }

}

static void desenhar_colisoes(Lista_t colisoes, SVG svg_saida) {
  for (Posic_t it = lt_get_first(colisoes); it != NULL; it = lt_get_next(colisoes, it)) {
    Colisao colisao = lt_get(colisoes, it);
    Figura figura = colisao_get_figura(colisao);
    escreve_comentario(svg_saida, "COLISAO");
    desenha_figura(svg_saida, figura, 1, false);

  }
}

static bool linhas_intersectam( Ponto2D_t p, Ponto2D_t r, Ponto2D_t q, Ponto2D_t s ){

  double rxs = p2d_vetorial( r, s );
  if(rxs == 0) return false;

  Ponto2D_t qmp = p2d_sub( q, p );

  // t = (q − p) × s / (r × s)
  double t = p2d_vetorial( qmp, s ) / rxs;

  // u = (q − p) × r / (r × s)
  double u = p2d_vetorial( qmp, r ) / rxs;

  if( between(t, 0, 1) && between(u, 0, 1) )
    return true;

  return false;
}

static bool aresta_corresponde_colisao(struct Controlador* controlador, ArestaInfo aresta, Figura fig_colisao ){
  
  VerticeInfo vertice_origem  = gd_get_info_vertice( controlador->mapa_viario, aresta->origem );
  VerticeInfo vertice_destino = gd_get_info_vertice( controlador->mapa_viario, aresta->destino );

  // Verificar se a reta da aresta cruza uma das retas dos lados dos retângulos
  
  Ponto2D_t origem  = vertice_origem->pos;
  Ponto2D_t destino = vertice_destino->pos;
  Ponto2D_t colisao = get_pos(fig_colisao);
  Ponto2D_t colisao_tamanho;

  colisao_tamanho.x = get_w( fig_colisao );
  colisao_tamanho.y = get_h( fig_colisao );

  // q e s Representarão cada lado do retângulo
  Ponto2D_t q;
  Ponto2D_t s;

  Ponto2D_t p = origem;
  Ponto2D_t r = p2d_sub( destino, origem );

  // Borda de cima
  q = colisao;
  s = p2d_new(colisao_tamanho.x, 0);
  if( linhas_intersectam( p, r, q, s ) ) return true;

  // Borda da esquerda
  q = colisao;
  s = p2d_new(0, colisao_tamanho.y);
  if( linhas_intersectam( p, r, q, s ) ) return true;

  // Borda de baixo
  q    = colisao;
  q.y += colisao_tamanho.y;
  s    = p2d_new(colisao_tamanho.x, 0);
  if( linhas_intersectam( p, r, q, s ) ) return true;

  // Borda da direita
  q    = colisao;
  q.x += colisao_tamanho.x;
  s    = p2d_new(0, colisao_tamanho.y);
  if( linhas_intersectam( p, r, q, s ) ) return true;

  return false;
}

static Lista_t pegar_aresta_correspondente(struct Controlador* controlador, Figura fig_colisao ) {
  Ponto2D_t colisao_centro = get_centro_massa( fig_colisao );
  KDTree_t arestas = controlador->arestas_mapa_viario;
  KDTree_t quadras = controlador->elementos[QUADRA];
  // Encontrar quadra mais próxima
  Elemento quadra = kdt_nearest_neighbor( quadras, &colisao_centro, __distancia_quadra_ponto ).point1;

  Ponto2D_t tamanho_quadra;
  tamanho_quadra.x = get_largura(quadra);
  tamanho_quadra.y = get_altura(quadra);

  tamanho_quadra = p2d_mult(tamanho_quadra, 0.5);

  // Pontos de busca
  Ponto2D_t ponto_a = p2d_sub(colisao_centro, tamanho_quadra);
  Ponto2D_t ponto_b = p2d_add(colisao_centro, tamanho_quadra);

  // Encontrar arestas num "raio" de uma quadra
  Lista_t lista_arestas = kdt_range_search( arestas, __aresta_dentro, &ponto_a, &ponto_b );
  Lista_t arestas_colisao = lt_create();

  figura_expandir(fig_colisao, 2, 2);
  
  // Comparar cada aresta
  while( lt_length( lista_arestas ) > 0 ){
    ArestaInfo info_aresta = lt_remove( lista_arestas, lt_get_first(lista_arestas) );
    if( aresta_corresponde_colisao( controlador, info_aresta, fig_colisao ) ){
      lt_insert( arestas_colisao, info_aresta );
    }
  }

  figura_expandir(fig_colisao, -2, -2);

  lt_destroy(lista_arestas, 0);

  return arestas_colisao;
  
}

int comando_qry_dc( void* _this, void* _controlador ){
  struct Comando *this = _this;
  struct Controlador *controlador = _controlador;

  limpar_colisoes(controlador->colisoes);
  // Colisões removidas

  Veiculo* vetor_veiculos = lt_to_array( controlador->veiculos );
  int tamanho = lt_length( controlador->veiculos );
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

        Lista_t arestas_colisao = pegar_aresta_correspondente( controlador, fig_colisao ) ;
        Colisao this_colisao  = colisao_create( fig_colisao, arestas_colisao );


        set_arestas_invalido( arestas_colisao );

        lt_insert( controlador->colisoes, this_colisao );
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