#include "controlador.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SVG.h"
#include "elemento.h"
#include "figura.h"
#include "modules/lista.h"
#include "modules/logger.h"
#include "utils.h"

#define TOTAL_FIGURAS_DEFAULT 1000

struct Controlador {
  Lista saida;
  Lista saida_qry;
  Lista saida_svg_qry;

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

  this->saida         = create_lista();
  this->saida_qry     = create_lista();
  this->saida_svg_qry = create_lista();

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
  int i = 1;
  char *texto_auxiliar;

  this = (struct Controlador *) c;

  while (i < argc) {
    if (!strcmp(argv[i], "-f")) {
      i++;

      this->nome_base = remover_extensao(argv[i]);
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

      this->arq_query = remover_extensao(argv[i]);
    }

    i++;
  }

  if (!this->nome_base) {
    LOG_PRINT(LOG_STDOUT, "Nao foi inserido um nome de arquivo.");
    exit(EXIT_FAILURE);
  }

  if (!this->dir_saida) {
    LOG_PRINT(LOG_STDOUT, "Nao foi inserido um diretorio de saida.");
    exit(EXIT_FAILURE);
  }

  LOG_PRINT(LOG_FILE, "Parametros sem problemas.");
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
  char *cor, *cor_borda, *saida, *sufixo, *cep;
  int id, id2, i, count, elemento_id;
  float x, y, x2, y2, r, h, w, distancia;
  size_t length;
  Figura figAtual, *newFiguras;
  Elemento new_elemento;
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
        LOG_PRINT(
          LOG_STDOUT,
          "Nao ha figura no id %d! (linha %d)",
          id + 1,
          this->linha_atual);
        return 0;
      }

      if (!this->figuras[id2]) {
        LOG_PRINT(
          LOG_STDOUT,
          "Nao ha figura no id %d! (linha %d)",
          id2 + 1,
          this->linha_atual);
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
        LOG_PRINT(
          LOG_STDOUT,
          "Nao ha figura no id %d! (linha %d)",
          id + 1,
          this->linha_atual);
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
        LOG_PRINT(
          LOG_STDOUT,
          "Nao ha figura no id %d! (linha %d)",
          id + 1,
          this->linha_atual);
        return 0;
      }

      if (!this->figuras[id2]) {
        LOG_PRINT(
          LOG_STDOUT,
          "Nao ha figura no id %d! (linha %d)",
          id2 + 1,
          this->linha_atual);
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
        LOG_PRINT(
          LOG_STDOUT,
          "Nao ha figura no id %d! (linha %d)",
          id + 1,
          this->linha_atual);
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
    case GEO_INSERE_QUADRA:
      cep = params[0];
      x   = strtof(params[1], &saida);
      y   = strtof(params[2], &saida);
      w   = strtof(params[3], &saida);
      h   = strtof(params[4], &saida);

      new_elemento = cria_quadra(x, y, cep, w, h);

      set_cor_elemento(new_elemento, this->cores[QUADRA]);
      set_cor_borda_elemento(new_elemento, this->cores_borda[QUADRA]);

      insert_lista(this->elementos[QUADRA], (Item) new_elemento);

      break;
    case GEO_INSERE_HIDRANTE:
      cep = params[0];
      x   = strtof(params[1], &saida);
      y   = strtof(params[2], &saida);

      new_elemento = cria_hidrante(x, y, cep);

      set_cor_elemento(new_elemento, this->cores[HIDRANTE]);
      set_cor_borda_elemento(new_elemento, this->cores_borda[HIDRANTE]);

      insert_lista(this->elementos[HIDRANTE], (Item) new_elemento);
      break;
    case GEO_INSERE_SEMAFORO:
      cep = params[0];
      x   = strtof(params[1], &saida);
      y   = strtof(params[2], &saida);

      new_elemento = cria_semaforo(x, y, cep);

      set_cor_elemento(new_elemento, this->cores[SEMAFORO]);
      set_cor_borda_elemento(new_elemento, this->cores_borda[SEMAFORO]);

      insert_lista(this->elementos[SEMAFORO], (Item) new_elemento);
      break;
    case GEO_INSERE_RADIO_BASE:
      cep = params[0];
      x   = strtof(params[1], &saida);
      y   = strtof(params[2], &saida);

      new_elemento = cria_radio_base(x, y, cep);

      set_cor_elemento(new_elemento, this->cores[RADIO_BASE]);
      set_cor_borda_elemento(new_elemento, this->cores_borda[RADIO_BASE]);

      insert_lista(this->elementos[RADIO_BASE], (Item) new_elemento);
      break;
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
    case QRY_BUSCA_RECT:
      x = strtof(params[0], NULL);
      y = strtof(params[1], NULL);
      w = strtof(params[2], NULL);
      h = strtof(params[3], NULL);

      figAtual = cria_retangulo(x, y, w, h, "transparent", "black");

      for (i = 0; i < 4; i++) {
        Lista listAtual = this->elementos[i];

        Posic iterator = get_first_lista(listAtual);

        while (iterator) {
          new_elemento = (Elemento) get_lista(listAtual, iterator);

          int contem;

          // Checar se retangulo esta dentro do outro
          if (get_tipo_elemento(new_elemento) == QUADRA)
            contem = dentro_figura(figAtual, get_figura_elemento(new_elemento));

          // Saber se o new_elemento está dentro da figAtual;
          else
            contem = contem_ponto(figAtual, get_x(new_elemento), get_y(new_elemento));

          if (contem) {
            saida = get_info_elemento(new_elemento);
            insert_lista(this->saida_qry, (Item) saida);
          }

          iterator = get_next_lista(listAtual, iterator);
        }
      }

      insert_lista(this->saida_svg_qry, (Item) figAtual);

      break;
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

  length = strlen(this->arq_query) + strlen(this->dir_entrada) + 2 + 4;

  path = (char *) malloc(length * sizeof(char));

  sprintf(path, "%s/%s.qry", this->dir_entrada, this->arq_query);

  arq = abrir_arquivo(path, LEITURA);

  while ((linha = ler_proxima_linha(arq))) {
    insert_lista(this->fila_execucao, (Item) cria_comando(linha));

    free(linha);
  }

  free(path);

  fechar_arquivo(arq);

  LOG_PRINT(LOG_FILE, "Fila de execução gerada com sucesso.");
}

/**
 * Essa função desenha e escreve os arquivos de devem fazer isso
 * É meio que uma função que deve ser chamada antes do programa
 * finalizar, mas depois de rodar todos os comandos.
 */
void finalizar_arquivos(Controlador c) {
  struct Controlador *this = (struct Controlador *) c;
  char *full_path;
  size_t length;
  Arquivo arq;
  Posic iterator;

  // Arquivo [nome_base]-[nome_qry].txt
  length    = strlen(this->nome_base) + strlen(this->dir_saida) + strlen(this->arq_query) + 1 + 1 + 4;
  full_path = calloc(length, sizeof(char));
  sprintf(full_path, "%s%s-%s.txt", this->dir_saida, this->nome_base, this->arq_query);

  arq = abrir_arquivo(full_path, ESCRITA);

  iterator = get_first_lista(this->saida_qry);
  while (iterator) {
    escrever_linha(arq, get_lista(this->saida_qry, iterator));
    iterator = get_next_lista(this->saida_qry, iterator);
  }

  free(full_path);
  fechar_arquivo(arq);

  // Arquivo [nome_base]-[nome_qry].svg
  SVG s;

  full_path = calloc(length, sizeof(char));
  sprintf(full_path, "%s%s-%s.svg", this->dir_saida, this->nome_base, this->arq_query);

  s = cria_SVG(full_path, 5000, 5000);

  iterator = get_first_lista(this->saida_svg_qry);
  while (iterator) {
    desenha_figura(s, get_lista(this->saida_svg_qry, iterator), 0.4, SVG_BORDA_TRACEJADA);
    iterator = get_next_lista(this->saida_svg_qry, iterator);
  }

  salva_SVG(s);

  free(full_path);
  destruir_SVG(s);
}

void destruir_controlador(Controlador c) {
  struct Controlador *this;
  int i;

  this = (struct Controlador *) c;

  destruir_lista(this->saida, NULL);
  destruir_lista(this->saida_qry, &free);
  destruir_lista(this->saida_svg_qry, &destruir_figura);

  i = 0;
  while (this->total_figuras) {
    if (this->figuras[i]) {
      destruir_figura(this->figuras[i]);
      this->total_figuras--;
    }
    i++;
  }
  free(this->figuras);

  // Sobreposicoes
  destruir_lista(this->sobreposicoes, &destruir_figura);

  // Elementos
  destruir_lista(this->elementos[QUADRA], &destruir_elemento);
  destruir_lista(this->elementos[HIDRANTE], &destruir_elemento);
  destruir_lista(this->elementos[SEMAFORO], &destruir_elemento);
  destruir_lista(this->elementos[RADIO_BASE], &destruir_elemento);

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

  destruir_lista(this->fila_execucao, NULL);

  free(c);
}

char *get_nome_base(Controlador c) {
  return ((struct Controlador *) c)->nome_base;
}

/** METODOS PRIVADOS */

static void desenhar_todas_figuras(Controlador c, SVG s) {
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
      desenha_figura(s, figAtual, 0.4, SVG_BORDA_SOLIDA);

      count++;
    }

    i++;
  }

  #ifdef DEBUG
  /* Printa os ids */

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

static void desenhar_sobreposicoes(Controlador c, SVG s) {
  struct Controlador *this;
  Figura figDash;

  this = (struct Controlador *) c;

  if (!length_lista(this->sobreposicoes))
    return;

  /* Calcular retangulo das sobreposicoes */
  Posic iterator = get_first_lista(this->sobreposicoes);

  while (iterator) {
    figDash = (Figura) get_lista(this->sobreposicoes, iterator);

    desenha_figura(s, figDash, 1.0, SVG_BORDA_TRACEJADA);
    escreve_texto(
      s, "sobrepoe", get_x(figDash), get_y(figDash) - 5, 15, "purple");

    iterator = get_next_lista(this->sobreposicoes, iterator);
  }
}

static void escrever_txt_final(Controlador c) {
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
