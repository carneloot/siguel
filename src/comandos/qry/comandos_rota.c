#include <comando.r>
#include <controlador.r>

#include <mapa_viario/vertice.h>
#include <mapa_viario/aresta.h>
#include <utils.h>
#include <modules/dijkstra.h>
#include <modules/logger.h>
#include <SVG.h>

#include "svg_path.h"

#ifdef DEBUG
#include <figura.h>
#endif

#include <float.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/** Funcao auxiliar da get_vertice_by_ponto */
double __distancia_vertice_ponto(void *_vertice_info, void *_ponto, int dim) {
  VerticeInfo vertice_info = (VerticeInfo) _vertice_info;

  Ponto2D_t ponto         = * (Ponto2D_t *) _ponto;
  Ponto2D_t ponto_vertice = vertice_info->pos;

  switch (dim) {
    case 0:  return sqr(ponto.x - ponto_vertice.x);
    case 1:  return sqr(ponto.y - ponto_vertice.y);
    case -1: return p2d_dist_squared(ponto, ponto_vertice);
  }

  return DBL_MAX;
}

/**
 * Retorna o vertice mais proximo do ponto passado
 */
static VerticeInfo get_vertice_by_ponto(KDTree_t vertices, Ponto2D_t ponto) {
  return kdt_nearest_neighbor(vertices, &ponto, __distancia_vertice_ponto).point1;
}

/** FUNCOES PARA USAR NA HORA DE ACHAR O CAMINHO */
static double get_distancia_aresta(void *_aresta_info) {
  ArestaInfo aresta_info = (ArestaInfo) _aresta_info;
  return aresta_info->comprimento;
}

static double get_tempo_aresta(void *_aresta_info) {
  ArestaInfo aresta_info = (ArestaInfo) _aresta_info;

  if (aresta_info->comprimento == 0)
    return 0;

  if (aresta_info->velocidade_media == 0)
    return DBL_MAX;

  return aresta_info->comprimento / aresta_info->velocidade_media;
}

static double get_distancia_vertices(void *_atual, void *_target) {
  VerticeInfo atual  = (VerticeInfo) _atual;
  VerticeInfo target = (VerticeInfo) _target;

  return p2d_dist_squared(atual->pos, target->pos);
}

/**
 * Retorna o caminho de origem_info até destino_info.
 * Se distancia for true usa a distancia, senao usa a velocidade
 */
static Lista_t get_caminho(GrafoD_t grafo, VerticeInfo origem_info, VerticeInfo destino_info, bool distancia) {
  Lista_t caminho;

  double (*funcao)(void *) = (distancia == true) ? get_distancia_aresta : get_tempo_aresta;

  caminho = astar(grafo, origem_info->id, destino_info->id, funcao, get_distancia_vertices);

  return caminho;
}

/**
 * Desenha linhas entre os vertices indicados por caminho de cor cor
 */
static void desenhar_caminho_svg(SVG svg, Lista_t caminho, GrafoD_t mapa, char *cor) {

  Lista_t pontos = lt_create();

  Posic_t it = lt_get_first(caminho);

  while (it) {
    char *label = lt_get(caminho, it);

    VerticeInfo vertice = gd_get_info_vertice(mapa, label);

    Ponto2D_t *ponto = calloc(1, sizeof(*ponto));
    ponto->x = vertice->pos.x;
    ponto->y = vertice->pos.y;

    lt_insert(pontos, ponto);

    it = lt_get_next(caminho, it);
  }

  void *path = cria_path(pontos, 5, 1, cor);

  Desenhavel desenhavel = cria_desenhavel(path, print_path, free_path);

  desenha_desenhavel(svg, desenhavel);

  desenhavel_destruir(desenhavel);

  lt_destroy(pontos, free);

}

/**
 * Coloca na saida strings com os nomes das ruas do caminho passado.
 */
static void escrever_caminho_txt(Lista_t saida, Lista_t caminho, GrafoD_t mapa) {

  char *ultima_rua = NULL;

  Posic_t it = lt_get_first(caminho);
  while (lt_get_next(caminho, it)) {
    Posic_t next_it = lt_get_next(caminho, it);

    char *label_prev = lt_get(caminho, lt_get_previous(caminho, it));
    char *label      = lt_get(caminho, it);
    char *label_next = lt_get(caminho, next_it);

    ArestaInfo rua = gd_get_info_aresta(mapa, label, label_next);

    // Ignorar os pontos
    if (!strcmp(rua->nome, ".")) {
      it = next_it;
      continue;
    }

    // Eh o primeiro
    if (ultima_rua == NULL) {
      lt_insert(saida, format_string("\tSiga pela rua %s\n", rua->nome));

      ultima_rua = rua->nome;
      it = next_it;
      continue;
    }

    // Se a rua atual for igual a ultima rua, ignorar
    if (!strcmp(ultima_rua, rua->nome)) {
      it = next_it;
      continue;
    }

    // Vai fazer uma curva
    VerticeInfo vertice_anterior = gd_get_info_vertice(mapa, label_prev);
    VerticeInfo vertice_atual    = gd_get_info_vertice(mapa, label);
    VerticeInfo vertice_proximo  = gd_get_info_vertice(mapa, label_next);

    Ponto2D_t pos_anterior = vertice_anterior->pos;
    Ponto2D_t pos_atual    = vertice_atual->pos;
    Ponto2D_t pos_proximo  = vertice_proximo->pos;

    int orientacao = p2d_orientation(pos_anterior, pos_atual, pos_proximo);

    // Se a orientacao for zero, esta em linha reta
    // Logo, nao precisa virar
    if (orientacao == 0) {
      it = next_it;
      continue;
    }

    char *direcao = (orientacao < 0) ? "esquerda" : "direita";

    char *saida_txt = format_string("\tvire a %s na rua %s\n", direcao, rua->nome);

    lt_insert(saida, saida_txt);


    ultima_rua = rua->nome;
    it = next_it;
  }

  lt_insert(saida, format_string("\n"));

}

/**
 * Desenha um circulo de cor cor nos ponto passado
 */
static void desenhar_circulo(SVG svg, Ponto2D_t ponto, char *cor) {
  #ifdef DEBUG
  Figura ponto1 = cria_circulo(ponto.x,  ponto.y,  5, cor, "transparent");

  desenha_figura(svg, ponto1, 1, FIG_BORDA_SOLIDA);

  destruir_figura(ponto1);
  #endif
}

/**
 * Comando:    p?
 * Parametros: (t|p sufixo) (D|T) r1 r2 [cor]
 * Descricao:  Qual o melhor trajeto entre a origem que
 *             esta no registrador r1 e o destino que esta
 *             no registrador r2. Caso o percurso peça a
 *             representacao pictorica, o trajeto deve ser 
 *             pintado com a cor especificada
 *             ATENCAO: o calculo do percurso deve levar
 *             em consideracao as interdicoes
 *             estabelecidas pela ultima deteccao
 */

int comando_qry_p(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char** params = this->params;
  
  // parâmetro que diz se quer representação pictória ou textual
  char* eh_pictorico = params[0];

  if( eh_pictorico[0] == 'p' ){
    // ================================= É pictórico =================================
    char* sufixo = params[1];
    int registrador_origem = strtol( params[3] + 1, NULL, 10);
    int registrador_destino = strtol( params[4] + 1, NULL, 10);

    Ponto2D_t origem = controlador->registradores[ registrador_origem ]; // ATENÇÃO, CONSIDERANDO OS REGISTRADORES COMO 1-10 JÁ QUE EXISTEM 11 REGISTRADORES
    Ponto2D_t destino = controlador->registradores[ registrador_destino ];

    VerticeInfo origem_info  = get_vertice_by_ponto(controlador->vertices_mapa_viario, origem);
    VerticeInfo destino_info = get_vertice_by_ponto(controlador->vertices_mapa_viario, destino);

    char* cor = params[5];

    Lista_t caminho;

    char* menor_tempo = params[2];
    if( menor_tempo[0] == 'T'){
      // Menor tempo
      caminho = get_caminho(controlador->mapa_viario, origem_info, destino_info, false);
    }else{
      // Menor distâbcia
      caminho = get_caminho(controlador->mapa_viario, origem_info, destino_info, true);
    }


    char *qry_nome = get_nome(controlador->extras[e_qry]);

    char *path = format_string("%s%s-%s-%s.svg",
      controlador->dir_saida,
      controlador->nome_base,
      qry_nome,
      sufixo);

    SVG svg_saida;
    svg_saida = cria_SVG(path, controlador->max_qry.x, controlador->max_qry.y);

    free(path);
    free(qry_nome);

    desenhar_elementos(controlador, svg_saida);
    desenhar_mapa_viario(controlador, svg_saida);
    desenhar_veiculos(controlador, svg_saida);
    
    desenhar_circulo(svg_saida, origem, cor);
    desenhar_circulo(svg_saida, destino, cor);

    if (caminho == NULL) {
      escreve_texto(svg_saida, "Caminho inexistente", origem, 20, cor);
    } else {
      desenhar_caminho_svg( svg_saida, caminho, controlador->mapa_viario, cor);

      lt_destroy(caminho, NULL);
    }

    salva_SVG(svg_saida);
    destruir_SVG(svg_saida);


    return 1;
  }

  // ================================ Não é pictórico, é textual ================================

  // Significa que o próximo parâmetro é o que define se
  //  é para pegar o caminho mais curto ou mais rápido
  char* menor_tempo = params[1];

  int registrador_origem = strtol( params[2] + 1, NULL, 10);
  int registrador_destino = strtol( params[3] + 1, NULL, 10);

  Ponto2D_t origem = controlador->registradores[ registrador_origem ]; // ATENÇÃO, CONSIDERANDO OS REGISTRADORES COMO 1-10 JÁ QUE EXISTEM 11 REGISTRADORES
  Ponto2D_t destino = controlador->registradores[ registrador_destino ];

  VerticeInfo origem_info = get_vertice_by_ponto( controlador->vertices_mapa_viario, origem );
  VerticeInfo destino_info = get_vertice_by_ponto( controlador->vertices_mapa_viario, destino );
  
  Lista_t caminho;

  if( menor_tempo[0] == 'T' ){
    caminho = get_caminho( controlador->mapa_viario, origem_info, destino_info, false );
  }else{
    caminho = get_caminho( controlador->mapa_viario, origem_info, destino_info, true );
  }

  if (caminho == NULL) {
    lt_insert(controlador->saida, format_string("Nao foi possivel gerar um caminho (comando %s):\n", this->string) );
    return 1;
  }


  lt_insert(controlador->saida, format_string( "Rota gerada pelo comando \"%s\":\n", this->string ) );

  escrever_caminho_txt(controlador->saida, caminho, controlador->mapa_viario);
  
  lt_destroy( caminho, NULL );
  // ================ VER SE TEM MAIS ALGO PARA COLOCAR AQUI ==============================================

  return 1;
}

/**
 * Comando:    sp?
 * Parametros: (t|p sufixo) (D|T) n r1 ... rn [c1 c2]
 * Descricao:  Semelhante ao comando p? . O percurso
 *             deve começar na posição r1 e finalizar em
 *             rn, passando (na sequência) pelas posições
 *             determinadas pelo registradores r2...rn-1.
 *             c1 e c2 são duas cores que devem ser
 *             usadas alternadamente cada um dos trechos
 *             da sequência de caminhos. Por exemplo, o
 *             trecho de r1 à r2 deve ser pintado com a
 *             cor c1; o trecho de r2 a r3, com a cor c2;
 *             de r3 à r4, com a cor c1, e assim
 *             sucessivamente.
 */

int comando_qry_sp(void *_this, void *_controlador) {
  struct Comando *this = _this;
  struct Controlador *controlador = _controlador;

  // Se o primeiro parametro for igual a p, pictorica = true
  bool pictorica = (this->params[0][0] == 'p');
  bool distancia;

  char *sufixo, *cor[2];

  int offset_total = 2;

  int total_registradores;

  if (pictorica) {
    sufixo = this->params[1];

    distancia = (this->params[2][0] == 'D');

    offset_total++;
    
    total_registradores = strtol(this->params[offset_total], NULL, 10);

    cor[0] = this->params[4 + total_registradores];
    cor[1] = this->params[4 + total_registradores + 1];
  } else {

    total_registradores = strtol(this->params[offset_total], NULL, 10);

    distancia = (this->params[1][0] == 'D');
  }

  SVG svg_saida;

  if (pictorica) {
    char *qry_nome = get_nome(controlador->extras[e_qry]);

    char *path = format_string("%s%s-%s-%s.svg",
      controlador->dir_saida,
      controlador->nome_base,
      qry_nome,
      sufixo);

    free(qry_nome);

    svg_saida = cria_SVG(path, controlador->max_qry.x, controlador->max_qry.y);

    free(path);

    desenhar_elementos(controlador, svg_saida);

    desenhar_mapa_viario(controlador, svg_saida);

    desenhar_veiculos(controlador, svg_saida);

  } else {
    lt_insert(controlador->saida,
      format_string("Rota gerada pelo comando \"%s\": \n", this->string));
  }

  offset_total++;

  for (int i = 0; i < total_registradores - 1; i++) {
    int r1 = strtol(this->params[offset_total + i]     + 1, NULL, 10);
    int r2 = strtol(this->params[offset_total + i + 1] + 1, NULL, 10);

    Ponto2D_t pos_origem  = controlador->registradores[r1];
    Ponto2D_t pos_destino = controlador->registradores[r2];

    VerticeInfo origem_info  = get_vertice_by_ponto(controlador->vertices_mapa_viario, pos_origem);
    VerticeInfo destino_info = get_vertice_by_ponto(controlador->vertices_mapa_viario, pos_destino);

    Lista_t caminho = get_caminho(controlador->mapa_viario, origem_info, destino_info, distancia);

    if (pictorica) {
      desenhar_circulo(svg_saida, pos_origem, cor[i % 2]);
      desenhar_circulo(svg_saida, pos_destino, cor[(i + 1) % 2]);
    }

    if (caminho == NULL) {
      if (pictorica) {

        escreve_texto(svg_saida, "Caminho inexistente", pos_origem, 20, cor[i % 2]);

      } else {
        lt_insert(controlador->saida,
          format_string("Nao foi possivel gerar um caminho de \"%s\" ate \"%s\".",
          origem_info->id, destino_info->id));
      }
      LOG_PRINT(LOG_STDOUT, "Nao foi possivel gerar um caminho de \"%s\" ate \"%s\".", origem_info->id, destino_info->id);
    } else {

      if (pictorica) {
        desenhar_caminho_svg(
          svg_saida,
          caminho,
          controlador->mapa_viario,
          cor[i % 2]);
      } else {
        escrever_caminho_txt(controlador->saida, caminho, controlador->mapa_viario);
      }

      lt_destroy(caminho, 0);
    }

  }

  if (pictorica) {
    salva_SVG(svg_saida);

    destruir_SVG(svg_saida);
  }

  return 1;
}