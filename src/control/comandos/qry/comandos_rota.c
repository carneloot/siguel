#include <model/comando.r>
#include <control/controlador.r>

#include <model/mapa_viario/vertice.h>
#include <model/mapa_viario/aresta.h>
#include <model/utils.h>
#include <model/modules/dijkstra.h>
#include <model/SVG.h>

#include <float.h>
#include <stdlib.h>
#include <stdbool.h>

double __distancia_vertice_ponto(const Item _vertice_info, const Item _ponto, int dim) {
  VerticeInfo vertice_info = (VerticeInfo) _vertice_info;

  Ponto2D ponto         = * (Ponto2D *) _ponto;
  Ponto2D ponto_vertice = vertice_info->pos;

  switch (dim) {
    case 0:  return mod(ponto.x - ponto_vertice.x);
    case 1:  return mod(ponto.y - ponto_vertice.y);
    case -1: return Ponto2D_t.dist_squared(ponto, ponto_vertice);
  }

  return DBL_MAX;
}

static VerticeInfo get_vertice_by_ponto(KDTree vertices, Ponto2D ponto) {
  return KDTree_t.nearest_neighbor(vertices, &ponto, __distancia_vertice_ponto).point1;
}

static double get_distancia_aresta(void *_aresta_info) {
  ArestaInfo aresta_info = (ArestaInfo) _aresta_info;
  return aresta_info->comprimento;
}

static double get_velocidade_media_aresta(void *_aresta_info) {
  ArestaInfo aresta_info = (ArestaInfo) _aresta_info;
  return 100.0 / aresta_info->velocidade_media;
}

static Lista get_caminho(GrafoD grafo, VerticeInfo origem_info, VerticeInfo destino_info, bool distancia) {
  Lista caminho;

  double (*funcao)(void *) = (distancia == true) ? get_distancia_aresta : get_velocidade_media_aresta;

  caminho = dijkstra(grafo, origem_info->id, destino_info->id, funcao);

  return caminho;
}

static void desenhar_caminho_svg(SVG svg, Lista caminho, GrafoD mapa, char *cor) {

  char *label_first         = Lista_t.get(caminho, Lista_t.get_first(caminho));
  VerticeInfo vertice_first = GrafoD_t.get_info_vertice(mapa, label_first);
  Ponto2D posicao_inicio    = vertice_first->pos;

  posicao_inicio.x += 10;
  posicao_inicio.y -= 10;

  escreve_texto(svg, "inicio", posicao_inicio, 25, cor);

  Posic it = Lista_t.get_first(caminho);

  while (Lista_t.get_next(caminho, it)) {
    Posic next_it = Lista_t.get_next(caminho, it);

    char *label      = Lista_t.get(caminho, it);
    char *label_next = Lista_t.get(caminho, next_it);

    VerticeInfo vertice      = GrafoD_t.get_info_vertice(mapa, label);
    VerticeInfo vertice_next = GrafoD_t.get_info_vertice(mapa, label_next);

    desenha_linha(svg, vertice->pos, vertice_next->pos, 1, 5, cor);
    
    it = next_it;
  }

  char *label_last         = Lista_t.get(caminho, Lista_t.get_last(caminho));
  VerticeInfo vertice_last = GrafoD_t.get_info_vertice(mapa, label_last);
  Ponto2D posicao_fim   = vertice_last->pos;

  posicao_fim.x += 10;
  posicao_fim.y -= 10;

  escreve_texto(svg, "fim", posicao_fim, 25, cor);
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
  // struct Comando *this = _this;
  // struct Controlador *controlador = _controlador;

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
    char *path = format_string("%s%s-%s.svg",
      controlador->dir_saida,
      controlador->nome_base,
      sufixo);

    svg_saida = cria_SVG(path, controlador->max_qry.x, controlador->max_qry.y);

    free(path);

    desenhar_elementos(controlador, svg_saida);
  }

  for (int i = 0; i < total_registradores - 1; i++) {
    int r1 = strtol(this->params[offset_total + 1 + i]     + 1, NULL, 10);
    int r2 = strtol(this->params[offset_total + 1 + i + 1] + 1, NULL, 10);

    Ponto2D pos_origem  = controlador->registradores[r1];
    Ponto2D pos_destino = controlador->registradores[r2];

    VerticeInfo origem_info  = get_vertice_by_ponto(controlador->vertices_mapa_viario, pos_origem);
    VerticeInfo destino_info = get_vertice_by_ponto(controlador->vertices_mapa_viario, pos_destino);

    Lista caminho = get_caminho(controlador->mapa_viario, origem_info, destino_info, distancia);

    if (pictorica) {
      desenhar_caminho_svg(
        svg_saida,
        caminho,
        controlador->mapa_viario,
        cor[i % 2]);
    }

    Lista_t.destruir(caminho, 0);
  }

  if (pictorica) {
    salva_SVG(svg_saida);

    destruir_SVG(svg_saida);
  }

  return 1;
}