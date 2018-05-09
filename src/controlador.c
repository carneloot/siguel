#include "controlador.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SVG.h"
#include "figura.h"
#include "utils.h"

#define TOTAL_FIGURAS_DEFAULT 1000

struct Controlador {
  Lista saida;
  char *nome_base;
  char *dir_saida;
  char *dir_entrada;

  Figura *figuras;
  int total_figuras;
  int max_figuras;

  int **sobrepos;
  int total_sobrepos;
};

void desenhar_todas_figuras(Controlador c, SVG s);

void desenhar_sobreposicoes(Controlador c, SVG s);

void escrever_txt_final(Controlador c);

void get_max_width_height(Controlador c, float *width, float *heigth);

/** METODOS PUBLICOS */

Controlador cria_controlador() {
  struct Controlador *this =
    (struct Controlador *) malloc(sizeof(struct Controlador));

  this->saida       = cria_lista();
  this->nome_base   = NULL;
  this->dir_saida   = NULL;
  this->dir_entrada = (char *) malloc(3 * sizeof(char));
  strcpy(this->dir_entrada, "./");

  this->max_figuras   = TOTAL_FIGURAS_DEFAULT;
  this->total_figuras = 0;
  this->figuras       = (Figura *) calloc(this->max_figuras, sizeof(Figura));

  this->total_sobrepos = 0;
  this->sobrepos       = (int **) malloc(this->max_figuras * sizeof(int *));

  return (void *) this;
}

void lidar_parametros(Controlador c, int argc, const char *argv[]) {
  struct Controlador *this;
  int i = 1, j, length;

  this = (struct Controlador *) c;

  while (i < argc) {
    if (!strcmp(argv[i], "-f")) {
      i++;
      length = strlen(argv[i]) - 4;

      this->nome_base = (char *) malloc((length + 1) * sizeof(char));
      for (j = 0; j < length; j++)
        this->nome_base[j] = argv[i][j];
      this->nome_base[j] = 0;
    }

    /* Pega o default directory */
    else if (!strcmp(argv[i], "-o")) {
      i++;
      this->dir_saida = (char *) malloc((strlen(argv[i]) + 1) * sizeof(char));
      strcpy(this->dir_saida, argv[i]);
      this->dir_saida = evaluate_dir(this->dir_saida);
    }

    else if (!strcmp(argv[i], "-e")) {
      i++;
      free(this->dir_entrada);
      this->dir_entrada = (char *) malloc((strlen(argv[i]) + 1) * sizeof(char));
      strcpy(this->dir_entrada, argv[i]);
      this->dir_entrada = evaluate_dir(this->dir_entrada);
    }

    i++;
  }

  /* TODO: Colocar erro caso nao tenha passo o -o e o -f */
}

int executar_comando(Controlador c, Comando com) {
  struct Controlador *this;
  enum TipoComando tipo;
  char **params;
  int result;

  /* Parametros especificos */
  char *cor, *cor_borda, *saida, *sufixo;
  int id, id2, i, count;
  float x, y, x2, y2, r, h, w, distancia;
  size_t length;
  Figura figAtual;
  SVG s;

  this   = (struct Controlador *) c;
  tipo   = get_tipo(com);
  params = get_parametros(com);
  result = 0;

  switch (tipo) {
    case MUDAR_NUM_FIGURAS:
      this->max_figuras = atoi(params[0]);
      this->figuras =
        (Figura *) realloc(this->figuras, this->max_figuras * sizeof(Figura));
      result = 1;
      break;

    case DESENHA_CIRCULO:
      id        = atoi(params[0]) - 1;
      cor_borda = params[1];
      cor       = params[2];
      r         = atof(params[3]);
      x         = atof(params[4]);
      y         = atof(params[5]);

      if (this->figuras[id]) {
        free(this->figuras[id]);
        this->total_figuras--;
      }

      this->figuras[id] = cria_circulo(x, y, r, cor, cor_borda);

      this->total_figuras++;

      result = 1;
      break;

    case DESENHA_RETANGULO:
      id        = atoi(params[0]) - 1;
      cor_borda = params[1];
      cor       = params[2];
      w         = atof(params[3]);
      h         = atof(params[4]);
      x         = atof(params[5]);
      y         = atof(params[6]);

      if (this->figuras[id]) {
        free(this->figuras[id]);
        this->total_figuras--;
      }

      this->figuras[id] = cria_retangulo(x, y, w, h, cor, cor_borda);

      this->total_figuras++;

      result = 1;
      break;

    case CHECA_SOBREPOSICAO:
      id  = atoi(params[0]) - 1;
      id2 = atoi(params[1]) - 1;

      length = 16;

      saida = (char *) malloc(length * sizeof(char));

      if (intercepta_figura(this->figuras[id], this->figuras[id2])) {
        sprintf(saida, "o %4d %4d\nSIM", id + 1, id2 + 1);
        /* Desenhar retangulo no lugar da sobreposicao */
        this->sobrepos[this->total_sobrepos] = (int *) malloc(2 * sizeof(int));

        this->sobrepos[this->total_sobrepos][0] = id;
        this->sobrepos[this->total_sobrepos][1] = id2;

        this->total_sobrepos++;
      } else
        sprintf(saida, "o %4d %4d\nNAO", id + 1, id2 + 1);

      inserir_lista(this->saida, (Item) saida);

      result = 1;

      break;

    case CHECA_PONTO:
      id = atoi(params[0]) - 1;
      x  = atof(params[1]);
      y  = atof(params[2]);

      length = 25;

      saida = (char *) malloc(length * sizeof(char));

      if (contem_ponto(this->figuras[id], x, y))
        sprintf(saida, "i %4d %4.1f %4.1f\nSIM", id + 1, x, y);
      else
        sprintf(saida, "i %4d %4.1f %4.1f\nNAO", id + 1, x, y);

      inserir_lista(this->saida, (Item) saida);

      result = 1;

      break;

    case DISTANCIA_FIGURAS:
      id  = atoi(params[0]) - 1;
      id2 = atoi(params[1]) - 1;

      distancia = distancia_figuras(this->figuras[id], this->figuras[id2]);

      length = 19;

      saida = (char *) malloc(length * sizeof(char));

      sprintf(saida, "d %4d %4d\n%4.1f", id + 1, id2 + 1, distancia);

      inserir_lista(this->saida, (Item) saida);

      result = 1;

      break;

    case CRIAR_SVG:
      id = atoi(params[0]) - 1;

      /* Cria o sufixo padrao caso nao tenha sido especificado */
      if (get_numero_parametros(com) > 1)
        sufixo = params[1];
      else
        sufixo = (char *) calloc(1, sizeof(char));

      length =
        strlen(this->nome_base) + strlen(this->dir_saida) + strlen(sufixo) + 6;

      saida = (char *) malloc(length * sizeof(char));

      sprintf(saida, "%s%s-%s.svg", this->dir_saida, this->nome_base, sufixo);

      get_max_width_height(c, &x, &y);

      s = cria_SVG(saida, x, y);

      free(saida);

      desenhar_todas_figuras(c, s);

      i     = 0;
      count = 1;

      get_centro_massa(this->figuras[id], &x, &y);

      while (count < this->total_figuras) {
        if (id == i) {
          i++;
          continue;
        }

        figAtual = this->figuras[i];

        if (figAtual) {
          get_centro_massa(figAtual, &x2, &y2);

          cor       = get_cor_borda(this->figuras[id]);
          distancia = distancia_figuras(this->figuras[id], figAtual);

          desenha_linha(s, x, y, x2, y2, cor);

          length = 9;

          saida = calloc(length, sizeof(char));

          sprintf(saida, "%6.1f", distancia);

          escreve_texto(s, saida, (x + x2) / 2 + 10, (y + y2) / 2, 15, cor);

          free(saida);

          count++;
        }

        i++;
      }

      salva_SVG(s);

      destruir_SVG(s);

      if (get_numero_parametros(com) <= 1)
        free(sufixo);

      result = 1;
      break;

    case FIM_DO_ARQUIVO:
      /* Escrever SVG final */
      length = strlen(this->nome_base) + strlen(this->dir_saida) + 5;

      saida = (char *) malloc(length * sizeof(char));

      sprintf(saida, "%s%s.svg", this->dir_saida, this->nome_base);

      get_max_width_height(c, &x, &y);

      s = cria_SVG(saida, x, y);

      desenhar_todas_figuras(c, s);

      desenhar_sobreposicoes(c, s);

      salva_SVG(s);

      destruir_SVG(s);
      free(saida);

      escrever_txt_final(c);

      result = 1;
      break;
    case NONE: result = 0; break;
  }

  return result;
}

Lista get_linhas_entrada(Controlador c) {
  struct Controlador *this;
  size_t tamanho_total;
  char *full_path, *linha_atual;
  Arquivo arq;
  Lista lista;

  this = (struct Controlador *) c;

  if (!this->nome_base)
    return NULL;

  tamanho_total = strlen(this->dir_entrada) + strlen(this->nome_base) + 1 + 4;
  full_path     = (char *) malloc(tamanho_total * sizeof(char));

  sprintf(full_path, "%s%s.geo", this->dir_entrada, this->nome_base);

  arq = abrir_arquivo(full_path, LEITURA);

  free(full_path);

  lista = cria_lista();

  while ((linha_atual = ler_proxima_linha(arq))) {
    inserir_lista(lista, (Item) linha_atual);
  }

  fechar_arquivo(arq);

  return lista;
}

void destruir_controlador(Controlador c) {
  struct Controlador *this;
  int i;

  this = (struct Controlador *) c;

  destruir_lista(this->saida);

  i = 0;
  while (this->total_figuras) {
    if (this->figuras[i]) {
      destruir_figura(this->figuras[i]);
      this->total_figuras--;
    }
    i++;
  }
  free(this->figuras);

  i = 0;
  while (i < this->total_sobrepos)
    free(this->sobrepos[i++]);
  free(this->sobrepos);

  if (this->nome_base)
    free(this->nome_base);
  if (this->dir_saida)
    free(this->dir_saida);
  if (this->dir_entrada)
    free(this->dir_entrada);

  free(c);
}

char *get_nome_base(Controlador c) {
  return ((struct Controlador *) c)->nome_base;
}

/** METODOS PRIVADOS */

void desenhar_todas_figuras(Controlador c, SVG s) {
  struct Controlador *this;

  int i, count;
  Figura figAtual;

  #ifdef DEBUG
  char *saida;
  float x, y;
  size_t length;
  #endif

  this  = (struct Controlador *) c;
  i     = 0;
  count = 0;

  while (count < this->total_figuras) {
    figAtual = this->figuras[i];

    if (figAtual) {
      desenha_figura(s, figAtual);

      count++;
    }

    i++;
  }

  /* Printa os ids */
  #ifdef DEBUG

  i = count = 0;

  while (count < this->total_figuras) {
    figAtual = this->figuras[i];

    if (figAtual) {
      /* Escrever numero */
      length = floor(log10(i + 1)) + 1 + 1;

      saida = (char *) malloc(length * sizeof(char));
      sprintf(saida, "%d", i + 1);
      get_centro_massa(figAtual, &x, &y);
      x -= 2;
      y += 2;
      escreve_texto(s, saida, x, y, 7, get_cor_borda(figAtual));

      free(saida);
    }

    count++;
    i++;
  }

  #endif
}

void desenhar_sobreposicoes(Controlador c, SVG s) {
  struct Controlador *this;
  Figura figDash, fig1, fig2;
  int i;

  this = (struct Controlador *) c;

  if (!this->total_sobrepos)
    return;

  /* Calcular retangulo das sobreposicoes */
  for (i = 0; i < this->total_sobrepos; i++) {
    fig1    = this->figuras[this->sobrepos[i][0]];
    fig2    = this->figuras[this->sobrepos[i][1]];
    figDash = get_rect_sobreposicao(fig1, fig2);

    desenha_dashed_rect(s, figDash);
    escreve_texto(
      s, "sobrepoe", get_x(figDash), get_y(figDash) - 5, 15, "purple");

    destruir_figura(figDash);
  }
}

void escrever_txt_final(Controlador c) {
  struct Controlador *this;
  size_t tamanho_total;
  char *full_path;
  Arquivo arq;
  Node node;

  this = (struct Controlador *) c;

  if (!this->nome_base)
    return;

  tamanho_total = strlen(this->dir_saida) + strlen(this->nome_base) + 1 + 4;
  full_path     = (char *) malloc(tamanho_total * sizeof(char));

  sprintf(full_path, "%s%s.txt", this->dir_saida, this->nome_base);

  arq = abrir_arquivo(full_path, ESCRITA);

  free(full_path);

  node = get_start_lista(this->saida);

  while (tem_proximo_node(node)) {
    escrever_linha(arq, (char *) get_proximo_node(&node));
  }

  fechar_arquivo(arq);
}

void get_max_width_height(Controlador c, float *w, float *h) {
  struct Controlador *this;
  Figura figAtual;
  float maxw, maxh, watual, hatual;
  int i, count;

  this = (struct Controlador *) c;
  maxw = maxh = 0;
  i = count = 0;

  while (count < this->total_figuras) {
    figAtual = this->figuras[i];

    if (!figAtual) {
      i++;
      continue;
    }

    watual = get_x(figAtual) + get_w(figAtual) + get_r(figAtual);
    hatual = get_y(figAtual) + get_h(figAtual) + get_r(figAtual);

    if (watual > maxw)
      maxw = watual;

    if (hatual > maxh)
      maxh = hatual;

    count++;
    i++;
  }

  *w = maxw + 4;
  *h = maxh + 4;
}