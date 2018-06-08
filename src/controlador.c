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

  int linha_atual;

  Lista sobreposicoes;

  float max_width;
  float max_height;

  Lista fila_execucao;
};

static void desenhar_todas_figuras(Controlador c, SVG s);

static void desenhar_sobreposicoes(Controlador c, SVG s);

static void escrever_txt_final(Controlador c);

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

  this->linha_atual = 0;

  this->sobreposicoes = cria_lista();

  this->max_width  = 0;
  this->max_height = 0;

  this->fila_execucao = cria_lista();

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

  if (!this->nome_base) {
    printf("Nao foi inserido um nome de arquivo.\nFechando programa.\n");
    exit(EXIT_FAILURE);
  }

  if (!this->dir_saida) {
    printf("Nao foi inserido um diretorio de saida.\nFechando programa.\n");
    exit(EXIT_FAILURE);
  }
}

int executar_comando(Controlador c) {
  struct Controlador *this = (struct Controlador *) c;
  Comando com;
  enum TipoComando tipo;
  char **params;

  com = (Comando) remover_inicio_lista(this->fila_execucao);

  /* Parametros especificos */
  char *cor, *cor_borda, *saida, *sufixo;
  int id, id2, i, count;
  float x, y, x2, y2, r, h, w, distancia;
  size_t length;
  Figura figAtual, *newFiguras;
  SVG s;

  tipo   = get_tipo(com);
  params = get_parametros(com);

  this->linha_atual++;

  switch (tipo) {
    case MUDAR_NUM_FIGURAS:
      this->max_figuras = atoi(params[0]);

      newFiguras = (Figura *) calloc(this->max_figuras, sizeof(Figura));

      i     = 0;
      count = 0;

      while (count < this->total_figuras) {
        figAtual = this->figuras[i];
        if (!figAtual) {
          i++;
          continue;
        }
        newFiguras[i] = this->figuras[i];
        count++;
        i++;
      }

      free(this->figuras);

      this->figuras = newFiguras;

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

      w = get_x(this->figuras[id]) + get_r(this->figuras[id]);
      h = get_y(this->figuras[id]) + get_r(this->figuras[id]);

      if (w > this->max_width)
        this->max_width = w + 4;

      if (h > this->max_height)
        this->max_height = h + 4;

      this->total_figuras++;

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

      w = get_x(this->figuras[id]) + get_w(this->figuras[id]);
      h = get_y(this->figuras[id]) + get_h(this->figuras[id]);

      if (w > this->max_width)
        this->max_width = w + 4;

      if (h > this->max_height)
        this->max_height = h + 4;

      this->total_figuras++;

      break;

    case CHECA_SOBREPOSICAO:
      id  = atoi(params[0]) - 1;
      id2 = atoi(params[1]) - 1;

      if (!this->figuras[id]) {
        printf(
          "Nao ha figura no id %d! (linha %d)\n", id + 1, this->linha_atual);
        return 0;
      }

      if (!this->figuras[id2]) {
        printf(
          "Nao ha figura no id %d! (linha %d)\n", id2 + 1, this->linha_atual);
        return 0;
      }

      length = 9 + strlen(params[0]) + strlen(params[1]);

      saida = (char *) malloc(length * sizeof(char));

      if (intercepta_figura(this->figuras[id], this->figuras[id2])) {
        sprintf(saida, "o %s %s\nSIM\n", params[0], params[1]);
        /* Desenhar retangulo no lugar da sobreposicao */
        inserir_lista(
          this->sobreposicoes,
          get_rect_sobreposicao(this->figuras[id], this->figuras[id2]));
      } else
        sprintf(saida, "o %s %s\nNAO\n", params[0], params[1]);

      inserir_lista(this->saida, (Item) saida);

      break;

    case CHECA_PONTO:
      id = atoi(params[0]) - 1;
      x  = atof(params[1]);
      y  = atof(params[2]);

      if (!this->figuras[id]) {
        printf(
          "Nao ha figura no id %d! (linha %d)\n", id + 1, this->linha_atual);
        return 0;
      }

      length = 10 + strlen(params[0]) + strlen(params[1]) + strlen(params[2]);

      saida = (char *) malloc(length * sizeof(char));

      if (contem_ponto(this->figuras[id], x, y))
        sprintf(saida, "i %s %s %s\nSIM\n", params[0], params[1], params[2]);
      else
        sprintf(saida, "i %s %s %s\nNAO\n", params[0], params[1], params[2]);

      inserir_lista(this->saida, (Item) saida);

      break;

    case DISTANCIA_FIGURAS:
      id  = atoi(params[0]) - 1;
      id2 = atoi(params[1]) - 1;

      if (!this->figuras[id]) {
        printf(
          "Nao ha figura no id %d! (linha %d)\n", id + 1, this->linha_atual);
        return 0;
      }

      if (!this->figuras[id2]) {
        printf(
          "Nao ha figura no id %d! (linha %d)\n", id2 + 1, this->linha_atual);
        return 0;
      }

      distancia = distancia_figuras(this->figuras[id], this->figuras[id2]);

      length = 12 + strlen(params[0]) + strlen(params[1]);

      saida = (char *) malloc(length * sizeof(char));

      sprintf(saida, "d %s %s\n%4.1f\n", params[0], params[1], distancia);

      inserir_lista(this->saida, (Item) saida);

      break;

    case CRIAR_SVG:
      id = atoi(params[0]) - 1;

      if (!this->figuras[id]) {
        printf(
          "Nao ha figura no id %d! (linha %d)\n", id + 1, this->linha_atual);
        return 0;
      }

      /* Cria o sufixo padrao caso nao tenha sido especificado */
      if (get_numero_parametros(com) > 1)
        sufixo = params[1];
      else
        sufixo = (char *) calloc(1, sizeof(char));

      length =
        strlen(this->nome_base) + strlen(this->dir_saida) + strlen(sufixo) + 6;

      saida = (char *) malloc(length * sizeof(char));

      sprintf(saida, "%s%s-%s.svg", this->dir_saida, this->nome_base, sufixo);

      s = cria_SVG(saida, this->max_width, this->max_height);

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

      break;

    case FIM_DO_ARQUIVO:
      /* Escrever SVG final */
      length = strlen(this->nome_base) + strlen(this->dir_saida) + 5;

      saida = (char *) malloc(length * sizeof(char));

      sprintf(saida, "%s%s.svg", this->dir_saida, this->nome_base);

      s = cria_SVG(saida, this->max_width, this->max_height);

      desenhar_todas_figuras(c, s);

      desenhar_sobreposicoes(c, s);

      salva_SVG(s);

      destruir_SVG(s);
      free(saida);

      escrever_txt_final(c);

      break;
    case NONE: break;
  }

  destruir_comando(com);

  return 1;
}

int ha_comandos(Controlador c) {
  struct Controlador *this = (struct Controlador *) c;

  return !lista_vazia(this->fila_execucao);
}

void gerar_fila_execucao(Controlador c) {
  // Lê os arquivos de entrada e coloca os comandos numa fila de comandos
  struct Controlador *this = (struct Controlador *) c;
  Arquivo arq;
  size_t length;
  char *path, *linha;

  length = strlen(this->nome_base) + strlen(this->dir_entrada) + 6;

  path = (char *) malloc(length * sizeof(char));

  sprintf(path, "%s/%s.geo", this->dir_entrada, this->nome_base);

  arq = abrir_arquivo(path, LEITURA);

  while ((linha = ler_proxima_linha(arq))) {
    inserir_lista(this->fila_execucao, (Item) cria_comando(linha));

    free(linha);
  }

  free(path);

  fechar_arquivo(arq);

  // Adicionar leitura do arquivo .qry aqui
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

  while (!lista_vazia(this->sobreposicoes))
    destruir_figura(remover_inicio_lista(this->sobreposicoes));

  destruir_lista(this->sobreposicoes);

  if (this->nome_base)
    free(this->nome_base);
  if (this->dir_saida)
    free(this->dir_saida);
  if (this->dir_entrada)
    free(this->dir_entrada);

  destruir_lista(this->fila_execucao);

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
  Figura figDash;
  Node no_atual;

  this = (struct Controlador *) c;

  if (lista_vazia(this->sobreposicoes))
    return;

  /* Calcular retangulo das sobreposicoes */
  no_atual = get_start_lista(this->sobreposicoes);

  while (tem_proximo_node(no_atual)) {
    figDash = (Figura) get_proximo_node(&no_atual);

    desenha_dashed_rect(s, figDash);
    escreve_texto(
      s, "sobrepoe", get_x(figDash), get_y(figDash) - 5, 15, "purple");
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
