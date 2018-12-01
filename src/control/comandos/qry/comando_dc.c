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

#define between(valor, min, max) ( ((valor) < (min)) ? false : ((valor) > (max)) ? false : true )

struct Colisao {
  Lista arestas_colisao;
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

static double __distancia_quadra_ponto(const Item _quadra, const Item _ponto, int dim) {
  Elemento quadra = (Elemento) _quadra;
  Ponto2D ponto_quadra = get_pos( quadra );
  Ponto2D ponto = * (Ponto2D *) _ponto;

  switch(dim){
    case  0: return sqr(ponto.x - ponto_quadra.x);
    case  1: return sqr(ponto.y - ponto_quadra.y);
    case -1: return Ponto2D_t.dist_squared( ponto, ponto_quadra );
  }

  return DBL_MAX;
}

static int __aresta_dentro(const Item value, int dim, const Item _ponto_a, const Item _ponto_b ){
  ArestaInfo aresta = (ArestaInfo) value;
  Ponto2D ponto_a = * (Ponto2D *) _ponto_a;
  Ponto2D ponto_b = * (Ponto2D *) _ponto_b;
  Ponto2D pos_aresta = aresta->pos;
  
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

static void limpar_colisoes(Lista colisoes) {
  // Limpar retornar todas as arestas às velocidades originais
  while ( Lista_t.length(colisoes) > 0 ) {
    struct Colisao* colisao = Lista_t.remove(
      colisoes, Lista_t.get_first(colisoes));

    //Arrumar cada aresta da lista de colisões
    while( Lista_t.length( colisao->arestas_colisao ) > 0 ){
      ArestaInfo aresta = Lista_t.remove( colisao->arestas_colisao, Lista_t.get_first(colisao->arestas_colisao) );
      set_aresta_valido(aresta);
    }
    Lista_t.destruir(colisao->arestas_colisao, NULL);
    destruir_figura(colisao->figura);
    free(colisao);
  }
}

static void set_arestas_invalido( Lista arestas ){

  while( Lista_t.length( arestas ) > 0 ){
    ArestaInfo aresta = Lista_t.remove( arestas, Lista_t.get_first(arestas) );
    set_aresta_invalido(aresta);
  }

}

static void desenhar_colisoes(Lista colisoes, SVG svg_saida) {
  for (Posic it = Lista_t.get_first(colisoes); it != NULL; it = Lista_t.get_next(colisoes, it)) {
    struct Colisao *colisao = Lista_t.get(colisoes, it);
    desenha_figura(svg_saida, colisao->figura, 1, false);

    for( Posic aresta = Lista_t.get_first(colisao->arestas_colisao); aresta != NULL; aresta = Lista_t.get_next(colisao->arestas_colisao, aresta) ){
      

      ArestaInfo info_aresta = Lista_t.get( colisao->arestas_colisao, aresta );
      escreve_comentario(svg_saida,
        "COLISAO",
        info_aresta->origem,
        info_aresta->destino);


      #ifdef DEBUG

      Figura fig_aresta = cria_circulo( info_aresta->pos.x, info_aresta->pos.y, 5, "cyan", "transparent" );
      desenha_figura( svg_saida, fig_aresta, 1, false );
      destruir_figura( fig_aresta );

      #endif
    }

  }
}

static bool linhas_intersectam( Ponto2D p, Ponto2D r, Ponto2D q, Ponto2D s ){

  double rxs = Ponto2D_t.vetorial( r,s );
  if(rxs == 0) return false;

  Ponto2D qmp = Ponto2D_t.sub( p, q );

  // t = (q − p) × s / (r × s)
  double t = Ponto2D_t.vetorial( qmp, s ) / rxs;

  // u = (q − p) × r / (r × s)
  double u = Ponto2D_t.vetorial( qmp, r ) / rxs;

  if( between(t, 0, 1) && between(u, 0, 1) )
    return true;

  return false;
}

static bool aresta_corresponde_colisao(struct Controlador* controlador, ArestaInfo aresta, Figura fig_colisao ){
  
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
  destino.y         = vertice_destino->pos.y;

  colisao.x         = get_x( fig_colisao );
  colisao.y         = get_y( fig_colisao );
  colisao_tamanho.x = get_w( fig_colisao );
  colisao_tamanho.y = get_h( fig_colisao );

  // q e s Representarão cada lado do retângulo
  Ponto2D q;
  Ponto2D s;

  Ponto2D r = Ponto2D_t.sub( destino, origem );

  // Borda de cima
  q = colisao;
  s.x = colisao.x + colisao_tamanho.x;
  s.y = colisao.y;
  s = Ponto2D_t.sub( s, q );
  if( linhas_intersectam( origem, r, q, s ) ) return true;

  // Borda da esquerda
  // q = colisao;
  s.x = colisao.x;
  s.y = colisao.y + colisao_tamanho.y;
  s = Ponto2D_t.sub( s, q );
  if( linhas_intersectam( origem, r, q, s) ) return true;

  // Borda de baixo
  q.x = colisao.x;
  q.y = colisao.y + colisao_tamanho.y;
  s.x = colisao.x + colisao_tamanho.x;
  s.y = colisao.y + colisao_tamanho.y;
  s = Ponto2D_t.sub( s, q );
  if( linhas_intersectam( origem, r, q, s ) )    return true;

  // Borda da direita
  q.x = colisao.x + colisao_tamanho.x;
  q.y = colisao.y;
  s.x = colisao.x + colisao_tamanho.x;
  s.y = colisao.y + colisao_tamanho.y;
  s = Ponto2D_t.sub( s, q );
  if( linhas_intersectam( origem, r, q, s ) )  return true;

  return false;
  
}

static Lista pegar_aresta_correspondente(struct Controlador* controlador, Figura fig_colisao ) {
  Ponto2D colisao_centro = get_centro_massa( fig_colisao );
  KDTree arestas = controlador->arestas_mapa_viario;
  KDTree quadras = controlador->elementos[QUADRA];
  // Encontrar quadra mais próxima
  Elemento quadra = KDTree_t.nearest_neighbor( quadras, &colisao_centro, __distancia_quadra_ponto ).point1;

  Ponto2D tamanho_quadra;
  tamanho_quadra.x = get_largura(quadra);
  tamanho_quadra.y = get_altura(quadra);

  tamanho_quadra = Ponto2D_t.mult(tamanho_quadra, 0.5);

  // Pontos de busca
  Ponto2D ponto_a = Ponto2D_t.sub(colisao_centro, tamanho_quadra);
  Ponto2D ponto_b = Ponto2D_t.add(colisao_centro, tamanho_quadra);

  // Encontrar arestas num "raio" de uma quadra
  Lista lista_arestas = KDTree_t.range_search( arestas, __aresta_dentro, &ponto_a, &ponto_b );
  Lista arestas_colisao = Lista_t.create();
  
  // Comparar cada aresta
  while( Lista_t.length( lista_arestas ) > 0 ){
    ArestaInfo info_aresta = Lista_t.remove( lista_arestas, Lista_t.get_first(lista_arestas) );
    if( aresta_corresponde_colisao( controlador, info_aresta, fig_colisao ) ){
      Lista_t.insert( arestas_colisao, info_aresta );
    }
  }

  Lista_t.destruir(lista_arestas, 0);

  return arestas_colisao;
  
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
        
        

        struct Colisao* this_colisao = malloc( sizeof(struct Colisao) );

        this_colisao->arestas_colisao = pegar_aresta_correspondente( controlador, fig_colisao ) ;
        this_colisao->figura          = fig_colisao;

        set_arestas_invalido( this_colisao->arestas_colisao );

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