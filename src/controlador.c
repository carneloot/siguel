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

static void desenhar_elementos(Controlador this, SVG svg);

static void escrever_txt_final(Controlador c);

static int elemento_dentro_figura(const Item _elemento, const void *_figura);

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

      this->max_width  = max(this->max_width, w + 4);
      this->max_height = max(this->max_height, h + 4);

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

      this->max_width  = max(this->max_width, w + 4);
      this->max_height = max(this->max_height, h + 4);

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

      desenhar_elementos(c, s);

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

      this->max_width  = max(this->max_width, x + w + 4);
      this->max_height = max(this->max_height, y + h + 4);

      new_elemento = cria_quadra(x, y, cep, w, h);

      set_cor_elemento(new_elemento, this->cores[QUADRA]);
      set_cor_borda_elemento(new_elemento, this->cores_borda[QUADRA]);

      insert_lista(this->elementos[QUADRA], (Item) new_elemento);

      break;
    case GEO_INSERE_HIDRANTE:
      cep = params[0];
      x   = strtof(params[1], &saida);
      y   = strtof(params[2], &saida);

      this->max_width  = max(this->max_width, x + RAIO_EQUIPAMENTOS + 4);
      this->max_height = max(this->max_height, y + RAIO_EQUIPAMENTOS + 4);

      new_elemento = cria_hidrante(x, y, cep);

      set_cor_elemento(new_elemento, this->cores[HIDRANTE]);
      set_cor_borda_elemento(new_elemento, this->cores_borda[HIDRANTE]);

      insert_lista(this->elementos[HIDRANTE], (Item) new_elemento);
      break;
    case GEO_INSERE_SEMAFORO:
      cep = params[0];
      x   = strtof(params[1], &saida);
      y   = strtof(params[2], &saida);

      this->max_width  = max(this->max_width, x + RAIO_EQUIPAMENTOS + 4);
      this->max_height = max(this->max_height, y + RAIO_EQUIPAMENTOS + 4);

      new_elemento = cria_semaforo(x, y, cep);

      set_cor_elemento(new_elemento, this->cores[SEMAFORO]);
      set_cor_borda_elemento(new_elemento, this->cores_borda[SEMAFORO]);

      insert_lista(this->elementos[SEMAFORO], (Item) new_elemento);
      break;
    case GEO_INSERE_RADIO_BASE:
      cep = params[0];
      x   = strtof(params[1], &saida);
      y   = strtof(params[2], &saida);

      this->max_width  = max(this->max_width, x + RAIO_EQUIPAMENTOS + 4);
      this->max_height = max(this->max_height, y + RAIO_EQUIPAMENTOS + 4);

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
    case QRY_BUSCA_CIRC:
    case QRY_BUSCA_RECT:
      saida = calloc(5, sizeof(char));
      if (tipo == QRY_BUSCA_RECT) {
        x = strtof(params[0], NULL);
        y = strtof(params[1], NULL);
        w = strtof(params[2], NULL);
        h = strtof(params[3], NULL);

        figAtual = cria_retangulo(x, y, w, h, "transparent", "black");

        this->max_width  = max(this->max_width, x + w + 4);
        this->max_height = max(this->max_height, y + h + 4);

        strcpy(saida, "q?:\n");
      } else {
        r = strtof(params[0], NULL);
        x = strtof(params[1], NULL);
        y = strtof(params[2], NULL);

        figAtual = cria_circulo(x, y, r, "transparent", "black");

        this->max_width  = max(this->max_width, x + r + 4);
        this->max_height = max(this->max_height, y + r + 4);

        strcpy(saida, "Q?:\n");
      }

      insert_lista(this->saida_qry, (Item) saida);

      for (i = 0; i < 4; i++) {
        Lista lista_atual = this->elementos[i];

        Posic iterator = get_first_lista(lista_atual);

        iterator =
          search_lista(lista_atual, iterator, figAtual, elemento_dentro_figura);

        while (iterator) {
          new_elemento = (Elemento) get_lista(lista_atual, iterator);

          saida = get_info_elemento(new_elemento);
          insert_lista(this->saida_qry, (Item) saida);

          iterator = search_lista(
            lista_atual,
            get_next_lista(lista_atual, iterator),
            figAtual,
            elemento_dentro_figura);
        }
      }

      insert_lista(this->saida_svg_qry, (Item) figAtual);

      break;
    case QRY_DELETE_QUADRA_RECT:
    case QRY_DELETE_QUADRA_CIRC:
      if (tipo == QRY_DELETE_QUADRA_RECT) {
        x = strtof(params[0], NULL);
        y = strtof(params[1], NULL);
        w = strtof(params[2], NULL);
        h = strtof(params[3], NULL);

        figAtual = cria_retangulo(x, y, w, h, "transparent", "black");

        this->max_width  = max(this->max_width, x + w + 4);
        this->max_height = max(this->max_height, y + h + 4);
      } else {
        r = strtof(params[0], NULL);
        x = strtof(params[1], NULL);
        y = strtof(params[2], NULL);

        figAtual = cria_circulo(x, y, r, "transparent", "black");

        this->max_width  = max(this->max_width, x + r + 4);
        this->max_height = max(this->max_height, y + r + 4);
      }

      Lista lista_atual = this->elementos[QUADRA];
      Posic iterator    = get_first_lista(lista_atual);
      Posic next_it;

      iterator =
        search_lista(lista_atual, iterator, figAtual, elemento_dentro_figura);

      while (iterator) {
        next_it = get_next_lista(lista_atual, iterator);

        new_elemento = get_lista(lista_atual, iterator);

        remove_lista(lista_atual, iterator);

        cep   = get_cep_elemento(new_elemento);
        saida = calloc(20 + strlen(cep), sizeof(char));
        sprintf(saida, "Quadra: %s deletada.\n", cep);  // 20 Caracteres
        insert_lista(this->saida_qry, (Item) saida);

        destruir_elemento(new_elemento);

        iterator =
          search_lista(lista_atual, next_it, figAtual, elemento_dentro_figura);
      }

      insert_lista(this->saida_svg_qry, (Item) figAtual);

      break;
    case QRY_DELETE_ALL_RECT:
    case QRY_DELETE_ALL_CIRC:
      if (tipo == QRY_DELETE_ALL_RECT) {
        x = strtof(params[1], NULL);
        y = strtof(params[2], NULL);
        w = strtof(params[3], NULL);
        h = strtof(params[4], NULL);

        figAtual = cria_retangulo(x, y, w, h, "transparent", "black");

        this->max_width  = max(this->max_width, x + w + 4);
        this->max_height = max(this->max_height, y + h + 4);
      } else {
        r = strtof(params[1], NULL);
        x = strtof(params[2], NULL);
        y = strtof(params[3], NULL);

        figAtual = cria_circulo(x, y, r, "transparent", "black");

        this->max_width  = max(this->max_width, x + r + 4);
        this->max_height = max(this->max_height, y + r + 4);
      }

      // Parsear params[0] para pegar variaveis
      int *tipos_pesquisa = (int *) calloc(4, sizeof(int));

      for (i = 0; i < strlen(params[0]); i++) {
        switch (params[0][i]) {
          case 'h': tipos_pesquisa[HIDRANTE] = 1; break;
          case 's': tipos_pesquisa[SEMAFORO] = 1; break;
          case 'r': tipos_pesquisa[RADIO_BASE] = 1; break;
        }
      }

      for (i = 1; i < 4; i++) {
        if (!tipos_pesquisa[i])
          continue;

        Lista lista_atual = this->elementos[i];
        Posic iterator    = get_first_lista(lista_atual);
        Posic next_it;

        iterator =
          search_lista(lista_atual, iterator, figAtual, elemento_dentro_figura);

        while (iterator) {
          next_it = get_next_lista(lista_atual, iterator);

          new_elemento = get_lista(lista_atual, iterator);

          remove_lista(lista_atual, iterator);

          sufixo = get_info_elemento(new_elemento);
          saida  = calloc(12 + strlen(sufixo), sizeof(char));
          sprintf(saida, "%s deletada.\n", sufixo);
          insert_lista(this->saida_qry, (Item) saida);

          destruir_elemento(new_elemento);

          iterator = search_lista(
            lista_atual, next_it, figAtual, elemento_dentro_figura);
        }
      }

      insert_lista(this->saida_svg_qry, (Item) figAtual);

      break;
    case QRY_MUDA_COR_QUADRA: break;
    case QRY_PRINT_EQUIPAMENTO: break;
    case QRY_CHECA_RADIO_BASE_PROXIMA: break;

    case COMENTARIO:
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
  Comando comando;

  length = strlen(this->nome_base) + strlen(this->dir_entrada) + 6;

  path = (char *) malloc(length * sizeof(char));

  sprintf(path, "%s/%s.geo", this->dir_entrada, this->nome_base);

  arq = abrir_arquivo(path, LEITURA);

  while ((linha = ler_proxima_linha(arq))) {
    comando = cria_comando(linha);

    // Comentarios nao sao inseridos na fila de execução
    if (get_tipo(comando) == COMENTARIO) {
      destruir_comando(comando);
      free(linha);
      continue;
    }

    insert_lista(this->fila_execucao, (Item) comando);

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

  char *qry_dir, *qry_file, *geo_file;

  if (!this->arq_query)
    return;

  qry_dir  = get_diretorio(this->arq_query);
  qry_file = get_nome(this->arq_query);
  geo_file = get_nome(this->nome_base);

  // Arquivo [nome_base]-[nome_qry].txt
  length = 7 + strlen(this->dir_saida) + strlen(qry_dir) + strlen(qry_file) +
           strlen(geo_file);
  full_path = calloc(length, sizeof(char));
  sprintf(
    full_path, "%s%s%s-%s.txt", this->dir_saida, qry_dir, geo_file, qry_file);

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
  sprintf(
    full_path, "%s%s%s-%s.svg", this->dir_saida, qry_dir, geo_file, qry_file);

  s = cria_SVG(full_path, this->max_width, this->max_height);

  desenhar_elementos(c, s);

  iterator = get_first_lista(this->saida_svg_qry);
  while (iterator) {
    desenha_figura(
      s, get_lista(this->saida_svg_qry, iterator), 0.4, SVG_BORDA_TRACEJADA);
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

  destruir_lista(this->saida, &free);
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

static void desenhar_elementos(Controlador _this, SVG svg) {
  struct Controlador *this = (struct Controlador *) _this;
  int i;
  Lista lista_atual;
  Posic iterator;
  Elemento elemento_atual;
  Figura figura_atual;

  // Para cada tipo de elemento
  for (i = 0; i < 4; i++) {
    lista_atual = this->elementos[i];

    // Itera pela lista e desesenha no SVG os elementos
    iterator = get_first_lista(lista_atual);
    while (iterator) {
      elemento_atual = (Elemento) get_lista(lista_atual, iterator);
      figura_atual   = get_figura_elemento(elemento_atual);

      desenha_figura(svg, figura_atual, 0.4, SVG_BORDA_SOLIDA);

      destruir_figura(figura_atual);

      iterator = get_next_lista(lista_atual, iterator);
    }
  }
}

static int elemento_dentro_figura(const Item _elemento, const void *_figura) {
  const Elemento elemento = (const Elemento) _elemento;
  const Figura figura     = (const Figura) _figura;

  int contem;

  // Se for quadra, checar a figura
  if (get_tipo_elemento(elemento) == QUADRA) {
    Figura figura_temp = get_figura_elemento(elemento);

    contem = dentro_figura(figura, figura_temp);

    destruir_figura(figura_temp);
  }

  // Se nao for quadra, comparar como ponto
  else
    contem = contem_ponto(figura, get_x(elemento), get_y(elemento));

  return !contem;
}