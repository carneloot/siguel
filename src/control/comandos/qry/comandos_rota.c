#include <model/comando.r>
#include <control/controlador.r>

#include <model/mapa_viario/vertice.h>
#include <model/mapa_viario/aresta.h>
#include <model/utils.h>
#include <model/modules/dijkstra.h>
#include <model/modules/logger.h>
#include <model/SVG.h>

#include "svg_path.h"

#ifdef DEBUG
#include <model/figura.h>
#endif

#include <float.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/** Funcao auxiliar da get_vertice_by_ponto */
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

/**
 * Retorna o vertice mais proximo do ponto passado
 */
static VerticeInfo get_vertice_by_ponto(KDTree vertices, Ponto2D ponto) {
  return KDTree_t.nearest_neighbor(vertices, &ponto, __distancia_vertice_ponto).point1;
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

/**
 * Retorna o caminho de origem_info até destino_info.
 * Se distancia for true usa a distancia, senao usa a velocidade
 */
static Lista get_caminho(GrafoD grafo, VerticeInfo origem_info, VerticeInfo destino_info, bool distancia) {
  Lista caminho;

  double (*funcao)(void *) = (distancia == true) ? get_distancia_aresta : get_tempo_aresta;

  caminho = dijkstra(grafo, origem_info->id, destino_info->id, funcao);

  return caminho;
}

/**
 * Escreve o texto no local do vertice indicado por label
 */
static void escrever_texto_vertice(SVG svg, GrafoD grafo, char *label, char *texto, char *cor) {
  VerticeInfo vertice = GrafoD_t.get_info_vertice(grafo, label);
  Ponto2D posicao     = vertice->pos;

  posicao.x += 10;
  posicao.y -= 10;

  escreve_texto(svg, texto, posicao, 25, cor);
}

/**
 * Desenha linhas entre os vertices indicados por caminho de cor cor
 */
static void desenhar_caminho_svg(SVG svg, Lista caminho, GrafoD mapa, char *cor) {

  Lista pontos = Lista_t.create();

  Posic it = Lista_t.get_first(caminho);

  while (it) {
    char *label = Lista_t.get(caminho, it);

    VerticeInfo vertice = GrafoD_t.get_info_vertice(mapa, label);

    Ponto2D *ponto = calloc(1, sizeof(*ponto));
    ponto->x = vertice->pos.x;
    ponto->y = vertice->pos.y;

    Lista_t.insert(pontos, ponto);

    it = Lista_t.get_next(caminho, it);
  }

  void *path = cria_path(pontos, 5, 1, cor);

  Desenhavel desenhavel = cria_desenhavel(path, print_path, free_path);

  desenha_desenhavel(svg, desenhavel);

  desenhavel_destruir(desenhavel);

  Lista_t.destruir(pontos, free);

}

/**
 * Coloca na saida strings com os nomes das ruas do caminho passado.
 */
static void escrever_caminho_txt(Lista saida, Lista caminho, GrafoD mapa) {
  Posic it = Lista_t.get_first(caminho);

  char *ultima_rua = ".";

  while (Lista_t.get_next(caminho, it)) {
    Posic next_it = Lista_t.get_next(caminho, it);

    char *label      = Lista_t.get(caminho, it);
    char *label_next = Lista_t.get(caminho, next_it);

    ArestaInfo rua = GrafoD_t.get_info_aresta(mapa, label, label_next);

    char *saida_txt = format_string("\tsiga pela rua %s\n", rua->nome);

    // Se a rua atual for diferente da ultima, mostra
    if (strcmp(rua->nome, ".")        == 0 ||
        strcmp(rua->nome, ultima_rua) == 0
    ) {
      free(saida_txt);
    } else {
      Lista_t.insert(saida, saida_txt);
      ultima_rua = rua->nome;
    }
    
    it = next_it;
  }

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

    #ifdef DEBUG
    desenhar_mapa_viario(controlador, svg_saida);
    #endif
  } else {
    Lista_t.insert(controlador->saida,
      format_string("Rota gerada: \n"));
  }

  offset_total++;

  for (int i = 0; i < total_registradores - 1; i++) {
    int r1 = strtol(this->params[offset_total + i]     + 1, NULL, 10);
    int r2 = strtol(this->params[offset_total + i + 1] + 1, NULL, 10);

    Ponto2D pos_origem  = controlador->registradores[r1];
    Ponto2D pos_destino = controlador->registradores[r2];

    VerticeInfo origem_info  = get_vertice_by_ponto(controlador->vertices_mapa_viario, pos_origem);
    VerticeInfo destino_info = get_vertice_by_ponto(controlador->vertices_mapa_viario, pos_destino);

    Lista caminho = get_caminho(controlador->mapa_viario, origem_info, destino_info, distancia);

    if (caminho == NULL) {
      if (pictorica) {

        escreve_texto(svg_saida, "Caminho inexistente", pos_origem, 20, cor[i % 2]);

      } else {
        Lista_t.insert(controlador->saida,
          format_string("Nao foi possivel gerar um caminho de \"%s\" ate \"%s\".",
          origem_info->id, destino_info->id));
      }
      LOG_PRINT(LOG_STDOUT, "Nao foi possivel gerar um caminho de \"%s\" ate \"%s\".", origem_info->id, destino_info->id);
    } else {

      if (pictorica) {
        #ifdef DEBUG
        Figura ponto1 = cria_circulo(pos_origem.x,  pos_origem.y,  10, cor[i % 2], "transparent");
        Figura ponto2 = cria_circulo(pos_destino.x, pos_destino.y, 10, cor[(i + 1) % 2], "transparent");

        desenha_figura(svg_saida, ponto1, 1, FIG_BORDA_SOLIDA);
        desenha_figura(svg_saida, ponto2, 1, FIG_BORDA_SOLIDA);

        destruir_figura(ponto1);
        destruir_figura(ponto2);
        #endif

        desenhar_caminho_svg(
          svg_saida,
          caminho,
          controlador->mapa_viario,
          cor[i % 2]);
      } else {
        escrever_caminho_txt(controlador->saida, caminho, controlador->mapa_viario);
      }

      Lista_t.destruir(caminho, 0);
    }

  }

  if (pictorica) {
    salva_SVG(svg_saida);

    destruir_SVG(svg_saida);
  }

  return 1;
}