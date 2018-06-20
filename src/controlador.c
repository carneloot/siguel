#include "controlador.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SVG.h"
#include "elemento.h"
#include "figura.h"
#include "modules/lista.h"
#include "utils.h"

#define TOTAL_FIGURAS_DEFAULT 1000

struct Controlador {
  Lista saida;
  char *nome_base;
  char *dir_saida;
  char *dir_entrada;
  char *arq_query;

  Figura *figuras;
  int total_figuras;
  int max_figuras;

  int linha_atual;

  Lista sobreposicoes;

  Lista elementos[4];
  char *cores[4];
  char *cores_borda[4];

  float max_width;
  float max_height;

  Lista fila_execucao;
};

static void desenhar_todas_figuras(Controlador c, SVG s);

static void desenhar_sobreposicoes(Controlador c, SVG s);

static void escrever_txt_final(Controlador c);

/** METODOS PUBLICOS */

Controlador cria_controlador() {
  int i;
  struct Controlador *this =
    (struct Controlador *) malloc(sizeof(struct Controlador));

  this->saida       = create_lista();
  this->nome_base   = NULL;
  this->dir_saida   = NULL;
  this->arq_query   = NULL;
  this->dir_entrada = (char *) malloc(3 * sizeof(char));
  strcpy(this->dir_entrada, "./");

  this->max_figuras   = TOTAL_FIGURAS_DEFAULT;
  this->total_figuras = 0;
  this->figuras       = (Figura *) calloc(this->max_figuras, sizeof(Figura));

  this->linha_atual = 0;

  this->sobreposicoes = create_lista();

  this->elementos[QUADRA]     = create_lista();
  this->elementos[HIDRANTE]   = create_lista();
  this->elementos[SEMAFORO]   = create_lista();
  this->elementos[RADIO_BASE] = create_lista();

  for (i = 0; i < 4; i++) {
    this->cores[i]       = NULL;
    this->cores_borda[i] = NULL;
  }

  this->max_width  = 0;
  this->max_height = 0;

  this->fila_execucao = create_lista();

  return (void *) this;
}

void lidar_parametros(Controlador c, int argc, const char *argv[]) {
  struct Controlador *this;
  int i = 1, j, length;
  char *texto_auxiliar;

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
      texto_auxiliar = (char *) malloc((strlen(argv[i]) + 1) * sizeof(char));
      strcpy(texto_auxiliar, argv[i]);

      this->dir_saida = evaluate_dir(texto_auxiliar);
      free(texto_auxiliar);
    }

    else if (!strcmp(argv[i], "-e")) {
      i++;
      texto_auxiliar = (char *) malloc((strlen(argv[i]) + 1) * sizeof(char));
      strcpy(texto_auxiliar, argv[i]);

      free(this->dir_entrada);
      this->dir_entrada = evaluate_dir(texto_auxiliar);
      free(texto_auxiliar);
    }

    else if (!strcmp(argv[i], "-q")) {
      i++;
      this->arq_query = (char *) malloc((strlen(argv[i]) + 1) * sizeof(char));
      strcpy(this->arq_query, argv[i]);
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

  Posic inicio_lista = get_first_lista(this->fila_execucao);

  com = (Comando) get_lista(this->fila_execucao, inicio_lista);
  remove_lista(this->fila_execucao, inicio_lista);

  /* Parametros especificos */
  char *cor, *cor_borda, *saida, *sufixo;
  int id, id2, i, count, elemento_id;
  float x, y, x2, y2, r, h, w, distancia;
  size_t length;
  Figura figAtual, *newFiguras;
  SVG s;

  tipo   = get_tipo(com);
  params = get_parametros(com);

  this->linha_atual++;

  switch (tipo) {
    case GEO_MUDAR_NUM_FIGURAS:
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

    case GEO_DESENHA_CIRCULO:
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

    case GEO_DESENHA_RETANGULO:
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

    case GEO_CHECA_SOBREPOSICAO:
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
        insert_lista(
          this->sobreposicoes,
          (Item) get_rect_sobreposicao(this->figuras[id], this->figuras[id2]));
      } else
        sprintf(saida, "o %s %s\nNAO\n", params[0], params[1]);

      insert_lista(this->saida, (Item) saida);

      break;

    case GEO_CHECA_PONTO:
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

      insert_lista(this->saida, (Item) saida);

      break;

    case GEO_DISTANCIA_FIGURAS:
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

      insert_lista(this->saida, (Item) saida);

      break;

    case GEO_CRIAR_SVG:
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

    case GEO_FIM_DO_ARQUIVO:
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
    // Inserir parte da cidade
    case GEO_INSERE_QUADRA: break;
    case GEO_INSERE_HIDRANTE: break;
    case GEO_INSERE_SEMAFORO: break;
    case GEO_INSERE_RADIO_BASE: break;
    case GEO_COR_QUADRA:
    case GEO_COR_HIDRANTE:
    case GEO_COR_RADIO_BASE:
    case GEO_COR_SEMAFORO:
      elemento_id = tipo - GEO_COR_QUADRA;

      if (this->cores[elemento_id])
        free(this->cores[elemento_id]);
      if (this->cores_borda[elemento_id])
        free(this->cores_borda[elemento_id]);

      this->cores[elemento_id] =
        (char *) malloc((strlen(params[1]) + 1) * sizeof(char));
      strcpy(this->cores[elemento_id], params[1]);

      this->cores_borda[elemento_id] =
        (char *) malloc((strlen(params[0]) + 1) * sizeof(char));
      strcpy(this->cores_borda[elemento_id], params[0]);
      break;

    // Comandos .qry
    case QRY_BUSCA_RECT: break;
    case QRY_BUSCA_CIRC: break;
    case QRY_DELETE_QUADRA_RECT: break;
    case QRY_DELETE_ALL_RECT: break;
    case QRY_DELETE_QUADRA_CIRC: break;
    case QRY_DELETE_ALL_CIRC: break;
    case QRY_MUDA_COR_QUADRA: break;
    case QRY_PRINT_EQUIPAMENTO: break;
    case QRY_CHECA_RADIO_BASE_PROXIMA: break;

    case NONE: break;
  }

  destruir_comando(com);

  return 1;
}

int ha_comandos(Controlador c) {
  struct Controlador *this = (struct Controlador *) c;

  return !!length_lista(this->fila_execucao);
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
    insert_lista(this->fila_execucao, (Item) cria_comando(linha));

    free(linha);
  }

  free(path);

  fechar_arquivo(arq);

  // Caso exista um arquivo .qry
  if (!this->arq_query)
    return;

  length = strlen(this->arq_query) + strlen(this->dir_entrada) + 2;

  path = (char *) malloc(length * sizeof(char));

  sprintf(path, "%s/%s", this->dir_entrada, this->arq_query);

  arq = abrir_arquivo(path, LEITURA);

  while ((linha = ler_proxima_linha(arq))) {
    insert_lista(this->fila_execucao, (Item) cria_comando(linha));

    free(linha);
  }

  free(path);

  fechar_arquivo(arq);
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

  Posic iterator = get_first_lista(this->sobreposicoes);

  while (iterator) {
    destruir_figura(get_lista(this->sobreposicoes, iterator));
    iterator = get_next_lista(this->sobreposicoes, iterator);
  }

  destruir_lista(this->sobreposicoes);

  destruir_lista(this->elementos[QUADRA]);
  destruir_lista(this->elementos[HIDRANTE]);
  destruir_lista(this->elementos[SEMAFORO]);
  destruir_lista(this->elementos[RADIO_BASE]);

  for (i = 0; i < 4; i++) {
    if (!this->cores[i])
      continue;

    free(this->cores[i]);
    free(this->cores_borda[i]);
  }

  if (this->nome_base)
    free(this->nome_base);
  if (this->dir_saida)
    free(this->dir_saida);
  if (this->dir_entrada)
    free(this->dir_entrada);
  if (this->arq_query)
    free(this->arq_query);

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
      desenha_figura(s, figAtual, 0.4, 0);

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

  this = (struct Controlador *) c;

  if (!length_lista(this->sobreposicoes))
    return;

  /* Calcular retangulo das sobreposicoes */
  Posic iterator = get_first_lista(this->sobreposicoes);

  while (iterator) {
    figDash = (Figura) get_lista(this->sobreposicoes, iterator);

    desenha_figura(s, figDash, 1.0, 1);
    escreve_texto(
      s, "sobrepoe", get_x(figDash), get_y(figDash) - 5, 15, "purple");

    iterator = get_next_lista(this->sobreposicoes, iterator);
  }
}

void escrever_txt_final(Controlador c) {
  struct Controlador *this;
  size_t tamanho_total;
  char *full_path;
  Arquivo arq;

  this = (struct Controlador *) c;

  if (!this->nome_base)
    return;

  tamanho_total = strlen(this->dir_saida) + strlen(this->nome_base) + 1 + 4;
  full_path     = (char *) malloc(tamanho_total * sizeof(char));

  sprintf(full_path, "%s%s.txt", this->dir_saida, this->nome_base);

  arq = abrir_arquivo(full_path, ESCRITA);

  free(full_path);

  Posic iterator = get_first_lista(this->saida);

  while (iterator) {
    escrever_linha(arq, (char *) get_lista(this->saida, iterator));
    iterator = get_next_lista(this->saida, iterator);
  }

  fechar_arquivo(arq);
}
