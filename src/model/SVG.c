#include "SVG.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <model/modules/ponto2d.h>
#include <model/arquivo.h>
#include <model/utils.h>

struct SVG {
  Arquivo saida;
  Ponto2D max;
};


#ifdef DEBUG

#define GARANTIA_TAMANHO 100
#define OFFSET_GRID 100

static void desenhar_grid(SVG _this) {
  struct SVG *this = (struct SVG *) _this;
  Ponto2D start, end;

  escreve_comentario(this, "GRID");

  for (float x = 0; x < this->max.x; x += OFFSET_GRID) {
    start = Ponto2D_t.new(x, 0);
    end   = Ponto2D_t.new(x, this->max.y);
    desenha_linha(this, start, end, 0.4, 1, "black");
  }

  for (float y = 0; y < this->max.y; y += OFFSET_GRID) {
    start = Ponto2D_t.new(0, y);
    end   = Ponto2D_t.new(this->max.x, y);
    desenha_linha(this, start, end, 0.4, 1, "black");
  }
}

#else

#define GARANTIA_TAMANHO 0

#endif


SVG cria_SVG(char *path, double max_width, double max_height) {
  struct SVG *this;

  this = (struct SVG *) malloc(sizeof(struct SVG));

  this->saida = abrir_arquivo(path, ESCRITA);
  this->max   = Ponto2D_t.new(max_width + GARANTIA_TAMANHO, max_height + GARANTIA_TAMANHO);

  escrever_linha(
    this->saida,
    "<svg width=\"%.1f\" height=\"%.1f\" "
    "xmlns=\"http://www.w3.org/2000/svg\">\n",
    this->max.x,
    this->max.y);

  escrever_linha(
    this->saida,
    "<defs>"
    "<marker id='arrowhead' orient='auto' markerWidth='2' markerHeight='4' refX='0.1' refY='2'>"
    "<path d='M0,0 V4 L2,2 Z' style='fill:black;' />"
    "</marker>"
    "</defs>\n"
  );

  return (SVG) this;
}

void desenha_figura(SVG s, Figura f, float opacity, int is_dashed) {
  struct SVG *this = (struct SVG *) s;

  set_opacity_figura(f, opacity);
  set_dashed_figura(f, is_dashed);
  char *fig_saida = get_svg_figura(f);
  escrever_linha(this->saida, fig_saida);
  free(fig_saida);
}

void desenha_elemento(SVG this, Elemento e) {
  Figura figura = get_figura_elemento(e);

  escreve_comentario(this, "%s: %s",
    get_tipo_string_elemento(get_tipo_elemento(e)), get_cep_elemento(e)
  );

  desenha_figura(this, figura, 0.4, FIG_BORDA_SOLIDA);

  // Se for quadra, desenhar o CEP no meio
  if (get_tipo_elemento(e) == QUADRA) {
    Ponto2D pos;
    char *cep;

    pos = get_pos(e);
    pos.x += 5;
    pos.y += get_altura(e) - 5;

    cep = get_cep_elemento(e);

    escreve_texto(this, cep, pos, 10, get_cor_borda(e));
  }

  destruir_figura(figura);
}

void desenha_asset(SVG _this, Ponto2D pos, Ponto2D size, char *nome) {
  struct SVG *this = (struct SVG *) _this;

  char *fullpath = format_string("%s%s", ASSETS_FOLDER, nome);

  Arquivo arq_asset  = abrir_arquivo(fullpath, LEITURA);
  char *asset_string = ler_arquivo_inteiro(arq_asset);
  fechar_arquivo(arq_asset);
  free(fullpath);

  escrever_linha(
    this->saida,
    "<svg width=\"%.2lf\" height=\"%.2lf\" x=\"%.2lf\" "
    "y=\"%.2lf\">\n%s</svg>\n",
    size.x,
    size.y,
    pos.x,
    pos.y,
    asset_string);

  free(asset_string);
}

void escreve_texto(SVG s, char *texto, Ponto2D pos, float tamanho, char *cor) {
  struct SVG *this;

  this = (struct SVG *) s;

  escrever_linha(
    this->saida,
    "<text x=\"%.1f\" y=\"%.1f\" "
    "style=\"fill:%s;font-size:%.1fpx;font-family:sans-serif\">%s</text>\n",
    pos.x,
    pos.y,
    cor,
    tamanho,
    texto);
}

void escreve_comentario(SVG _this, char *_texto, ...) {
  struct SVG * this = (struct SVG *) _this;

  char *texto = format_string("<!-- %s -->\n", _texto);

  va_list args;
  va_start(args, _texto);
  
  escrever_linha_args(this->saida, texto, args);

  va_end(args);

  free(texto);
}

void desenha_linha(SVG s, Ponto2D a, Ponto2D b, float opacity, double tamanho, char *cor) {
  struct SVG *this;

  this = (struct SVG *) s;

  escrever_linha(
    this->saida,
    "<line x1=\"%.1f\" y1=\"%.1f\" x2=\"%.1f\" y2=\"%.1f\" "
    "style=\"stroke:%s;stroke-width:%.1f;opacity:%.1f\" />\n",
    a.x,
    a.y,
    b.x,
    b.y,
    (!!cor) ? cor : "black",
    tamanho,
    opacity);
}

void desenha_desenhavel(SVG _this, Desenhavel desenhavel) {
  struct SVG *this = (struct SVG *) _this;

  char *text = desenhavel->to_svg(desenhavel->valor);

  escrever_linha(this->saida, text);

  free(text);
}

void salva_SVG(SVG s) {
  struct SVG *this = (struct SVG *) s;

  #ifdef DEBUG
  desenhar_grid(this);
  #endif

  escrever_linha(this->saida, "</svg>\n");

  fechar_arquivo(this->saida);
}

void destruir_SVG(SVG s) {
  free(s);
}