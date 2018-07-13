#include "controlador.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SVG.h"
#include "elemento.h"
#include "figura.h"
#include "modules/closest.h"
#include "modules/lista.h"
#include "modules/logger.h"
#include "utils.h"

#define RAIO_RADIOS_PROXIMOS 25

struct Controlador {
  Lista saida;
  Lista saida_svg_qry;

  char *nome_base;
  char *dir_saida;
  char *dir_entrada;
  char *arq_query;

  Lista figuras;

  int linha_atual;

  Lista sobreposicoes;

  Lista elementos[4];
  char *cores[4];
  char *cores_borda[4];

  float max_width_geo;
  float max_height_geo;

  float max_width_qry;
  float max_height_qry;

  Lista fila_execucao;
};

static void desenhar_todas_figuras(Controlador c, SVG s);

static void desenhar_sobreposicoes(Controlador c, SVG s);

static void desenhar_elementos(Controlador this, SVG svg);

static void escrever_txt_final(Controlador c);

static int elemento_dentro_figura(const Item _elemento, const void *_figura);

static int checar_id_figura(const Item _figura, const void *_id);

static int checar_id_elemento(const Item _elemento, const void *_id);

/** METODOS PUBLICOS */

Controlador cria_controlador() {
  int i;
  struct Controlador *this =
    (struct Controlador *) malloc(sizeof(struct Controlador));

  this->saida         = Lista_t.create();
  this->saida_svg_qry = Lista_t.create();

  this->nome_base   = NULL;
  this->dir_saida   = NULL;
  this->arq_query   = NULL;
  this->dir_entrada = (char *) malloc(3 * sizeof(char));
  strcpy(this->dir_entrada, "./");

  this->figuras = Lista_t.create();

  this->linha_atual = 0;

  this->sobreposicoes = Lista_t.create();

  this->elementos[QUADRA]     = Lista_t.create();
  this->elementos[HIDRANTE]   = Lista_t.create();
  this->elementos[SEMAFORO]   = Lista_t.create();
  this->elementos[RADIO_BASE] = Lista_t.create();

  for (i = 0; i < 4; i++) {
    this->cores[i]       = NULL;
    this->cores_borda[i] = NULL;
  }

  this->max_width_geo  = 0;
  this->max_height_geo = 0;

  this->max_width_qry  = 0;
  this->max_height_qry = 0;

  this->fila_execucao = Lista_t.create();

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
    LOG_ERRO("Nao foi inserido um nome de arquivo.");
    exit(EXIT_FAILURE);
  }

  if (!this->dir_saida) {
    LOG_ERRO("Nao foi inserido um diretorio de saida.");
    exit(EXIT_FAILURE);
  }

  if (!this->arq_query)
    LOG_PRINT(LOG_FILE, "Arquivo  \"%s.geo\" iniciado.", this->nome_base);
  else
    LOG_PRINT(
      LOG_FILE,
      "Arquivos \"%s.geo\" e \"%s.qry\" iniciados.",
      this->nome_base,
      this->arq_query);
}

int executar_proximo_comando(Controlador c) {
  struct Controlador *this = (struct Controlador *) c;
  Comando com;
  enum TipoComando tipo;
  char **params;

  Posic inicio_lista = Lista_t.get_first(this->fila_execucao);

  com = (Comando) Lista_t.get(this->fila_execucao, inicio_lista);
  Lista_t.remove(this->fila_execucao, inicio_lista);

  /* Parametros especificos */
  char *cor, *cor_borda, *saida, *sufixo, *cep;
  int id, id2, i, elemento_id;
  float x, y, x2, y2, r, h, w, distancia;
  size_t length;
  Figura figAtual;
  Figura figura1, figura2;

  Posic posic_figura1, posic_figura2;
  Posic iterator;

  Elemento new_elemento;
  SVG s;

  tipo   = get_tipo(com);
  params = get_parametros(com);

  this->linha_atual++;

  switch (tipo) {
    case GEO_MUDAR_NUM_FIGURAS: break;
    case GEO_DESENHA_CIRCULO:
      id        = strtol(params[0], NULL, 10);
      cor_borda = params[1];
      cor       = params[2];
      r         = strtof(params[3], NULL);
      x         = strtof(params[4], NULL);
      y         = strtof(params[5], NULL);

      figAtual = cria_circulo(x, y, r, cor, cor_borda);
      set_id_figura(figAtual, id);

      Lista_t.insert(this->figuras, figAtual);

      w = get_x(figAtual) + get_r(figAtual);
      h = get_y(figAtual) + get_r(figAtual);

      this->max_width_geo  = max(this->max_width_geo, w + 4);
      this->max_height_geo = max(this->max_height_geo, h + 4);

      break;

    case GEO_DESENHA_RETANGULO:
      id        = strtol(params[0], NULL, 10);
      cor_borda = params[1];
      cor       = params[2];
      w         = strtof(params[3], NULL);
      h         = strtof(params[4], NULL);
      x         = strtof(params[5], NULL);
      y         = strtof(params[6], NULL);

      figAtual = cria_retangulo(x, y, w, h, cor, cor_borda);
      set_id_figura(figAtual, id);

      Lista_t.insert(this->figuras, figAtual);

      w = get_x(figAtual) + get_w(figAtual);
      h = get_y(figAtual) + get_h(figAtual);

      this->max_width_geo  = max(this->max_width_geo, w + 4);
      this->max_height_geo = max(this->max_height_geo, h + 4);

      break;

    case GEO_CHECA_SOBREPOSICAO:
      id  = (int) strtol(params[0], NULL, 10);
      id2 = (int) strtol(params[1], NULL, 10);

      posic_figura1 = posic_figura2 = Lista_t.get_first(this->figuras);

      posic_figura1 =
        Lista_t.search(this->figuras, posic_figura1, &id, checar_id_figura);
      if (!posic_figura1) {
        LOG_ERRO("Nao ha figura no id %d! (linha %d)", id, this->linha_atual);
        break;
      }

      posic_figura2 =
        Lista_t.search(this->figuras, posic_figura2, &id2, checar_id_figura);
      if (!posic_figura2) {
        LOG_ERRO("Nao ha figura no id %d! (linha %d)", id2, this->linha_atual);
        break;
      }

      figura1 = Lista_t.get(this->figuras, posic_figura1);
      figura2 = Lista_t.get(this->figuras, posic_figura2);

      length = 9 + strlen(params[0]) + strlen(params[1]);

      saida = (char *) malloc(length * sizeof(char));

      if (sobrepoe_figura(figura1, figura2)) {
        sprintf(saida, "o %s %s\nSIM\n", params[0], params[1]);
        /* Desenhar retangulo no lugar da sobreposicao */
        Lista_t.insert(
          this->sobreposicoes, (Item) get_rect_sobreposicao(figura1, figura2));
      } else
        sprintf(saida, "o %s %s\nNAO\n", params[0], params[1]);

      Lista_t.insert(this->saida, (Item) saida);

      break;

    case GEO_CHECA_PONTO:
      id = (int) strtol(params[0], NULL, 10);
      x  = strtof(params[1], NULL);
      y  = strtof(params[2], NULL);

      posic_figura1 = Lista_t.get_first(this->figuras);
      posic_figura1 =
        Lista_t.search(this->figuras, posic_figura1, &id, checar_id_figura);

      if (!posic_figura1) {
        LOG_ERRO("Nao ha figura no id %d! (linha %d)", id, this->linha_atual);
        break;
      }

      figura1 = Lista_t.get(this->figuras, posic_figura1);

      length = 10 + strlen(params[0]) + strlen(params[1]) + strlen(params[2]);

      saida = (char *) malloc(length * sizeof(char));

      if (contem_ponto(figura1, x, y))
        sprintf(saida, "i %s %s %s\nSIM\n", params[0], params[1], params[2]);
      else
        sprintf(saida, "i %s %s %s\nNAO\n", params[0], params[1], params[2]);

      Lista_t.insert(this->saida, (Item) saida);

      break;

    case GEO_DISTANCIA_FIGURAS:
      id  = (int) strtol(params[0], NULL, 10);
      id2 = (int) strtol(params[1], NULL, 10);

      posic_figura1 = posic_figura2 = Lista_t.get_first(this->figuras);

      posic_figura1 =
        Lista_t.search(this->figuras, posic_figura1, &id, checar_id_figura);
      if (!posic_figura1) {
        LOG_ERRO("Nao ha figura no id %d! (linha %d)", id, this->linha_atual);
        break;
      }

      posic_figura2 =
        Lista_t.search(this->figuras, posic_figura2, &id2, checar_id_figura);
      if (!posic_figura2) {
        LOG_ERRO("Nao ha figura no id %d! (linha %d)", id2, this->linha_atual);
        break;
      }

      figura1 = Lista_t.get(this->figuras, posic_figura1);
      figura2 = Lista_t.get(this->figuras, posic_figura2);

      distancia = distancia_figuras(figura1, figura2);

      length = 12 + strlen(params[0]) + strlen(params[1]);

      saida = (char *) malloc(length * sizeof(char));

      sprintf(saida, "d %s %s\n%4.1f\n", params[0], params[1], distancia);

      Lista_t.insert(this->saida, (Item) saida);

      break;

    case GEO_CRIAR_SVG:
      id = (int) strtol(params[0], NULL, 10);

      posic_figura1 = Lista_t.get_first(this->figuras);
      posic_figura1 =
        Lista_t.search(this->figuras, posic_figura1, &id, checar_id_figura);

      if (!posic_figura1) {
        LOG_ERRO("Nao ha figura no id %d! (linha %d)", id, this->linha_atual);
        break;
      }

      figura1 = Lista_t.get(this->figuras, posic_figura1);

      /* Cria o sufixo padrao caso nao tenha sido especificado */
      if (get_numero_parametros(com) > 1)
        sufixo = params[1];
      else
        sufixo = (char *) calloc(1, sizeof(char));

      length =
        strlen(this->nome_base) + strlen(this->dir_saida) + strlen(sufixo) + 6;

      saida = (char *) malloc(length * sizeof(char));

      sprintf(saida, "%s%s-%s.svg", this->dir_saida, this->nome_base, sufixo);

      s = cria_SVG(saida, this->max_width_geo, this->max_height_geo);

      free(saida);

      desenhar_todas_figuras(c, s);

      i = 0;

      get_centro_massa(figura1, &x, &y);

      iterator = Lista_t.get_first(this->figuras);

      while (iterator) {
        figura2 = Lista_t.get(this->figuras, iterator);

        if (figura1 == figura2) {
          iterator = Lista_t.get_next(this->figuras, iterator);
          continue;
        }

        get_centro_massa(figura2, &x2, &y2);

        cor       = get_cor_borda(figura1);
        distancia = distancia_figuras(figura1, figura2);

        desenha_linha(s, x, y, x2, y2, cor);

        length = 9;

        saida = calloc(length, sizeof(char));

        sprintf(saida, "%6.1f", distancia);

        escreve_texto(s, saida, (x + x2) / 2 + 10, (y + y2) / 2, 15, cor);

        free(saida);

        iterator = Lista_t.get_next(this->figuras, iterator);
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

      s = cria_SVG(saida, this->max_width_geo, this->max_height_geo);

      desenhar_todas_figuras(c, s);

      desenhar_sobreposicoes(c, s);

      desenhar_elementos(c, s);

      salva_SVG(s);

      destruir_SVG(s);
      free(saida);

      break;
    // Inserir parte da cidade
    case GEO_INSERE_QUADRA:
      cep = params[0];
      x   = strtof(params[1], &saida);
      y   = strtof(params[2], &saida);
      w   = strtof(params[3], &saida);
      h   = strtof(params[4], &saida);

      this->max_width_geo  = max(this->max_width_geo, x + w + 4);
      this->max_height_geo = max(this->max_height_geo, y + h + 4);

      this->max_width_qry  = max(this->max_width_qry, x + w + 4);
      this->max_height_qry = max(this->max_height_qry, y + h + 4);

      new_elemento = cria_quadra(x, y, cep, w, h);

      set_cor_elemento(new_elemento, this->cores[QUADRA]);
      set_cor_borda_elemento(new_elemento, this->cores_borda[QUADRA]);

      Lista_t.insert(this->elementos[QUADRA], (Item) new_elemento);

      break;
    case GEO_INSERE_HIDRANTE:
      cep = params[0];
      x   = strtof(params[1], &saida);
      y   = strtof(params[2], &saida);

      this->max_width_geo = max(this->max_width_geo, x + RAIO_EQUIPAMENTOS + 4);
      this->max_height_geo =
        max(this->max_height_geo, y + RAIO_EQUIPAMENTOS + 4);

      this->max_width_qry = max(this->max_width_qry, x + RAIO_EQUIPAMENTOS + 4);
      this->max_height_qry =
        max(this->max_height_qry, y + RAIO_EQUIPAMENTOS + 4);

      new_elemento = cria_hidrante(x, y, cep);

      set_cor_elemento(new_elemento, this->cores[HIDRANTE]);
      set_cor_borda_elemento(new_elemento, this->cores_borda[HIDRANTE]);

      Lista_t.insert(this->elementos[HIDRANTE], (Item) new_elemento);
      break;
    case GEO_INSERE_SEMAFORO:
      cep = params[0];
      x   = strtof(params[1], &saida);
      y   = strtof(params[2], &saida);

      this->max_width_geo = max(this->max_width_geo, x + RAIO_EQUIPAMENTOS + 4);
      this->max_height_geo =
        max(this->max_height_geo, y + RAIO_EQUIPAMENTOS + 4);

      this->max_width_qry = max(this->max_width_qry, x + RAIO_EQUIPAMENTOS + 4);
      this->max_height_qry =
        max(this->max_height_qry, y + RAIO_EQUIPAMENTOS + 4);

      new_elemento = cria_semaforo(x, y, cep);

      set_cor_elemento(new_elemento, this->cores[SEMAFORO]);
      set_cor_borda_elemento(new_elemento, this->cores_borda[SEMAFORO]);

      Lista_t.insert(this->elementos[SEMAFORO], (Item) new_elemento);
      break;
    case GEO_INSERE_RADIO_BASE:
      cep = params[0];
      x   = strtof(params[1], &saida);
      y   = strtof(params[2], &saida);

      this->max_width_geo = max(this->max_width_geo, x + RAIO_EQUIPAMENTOS + 4);
      this->max_height_geo =
        max(this->max_height_geo, y + RAIO_EQUIPAMENTOS + 4);

      this->max_width_qry = max(this->max_width_qry, x + RAIO_EQUIPAMENTOS + 4);
      this->max_height_qry =
        max(this->max_height_qry, y + RAIO_EQUIPAMENTOS + 4);

      new_elemento = cria_radio_base(x, y, cep);

      set_cor_elemento(new_elemento, this->cores[RADIO_BASE]);
      set_cor_borda_elemento(new_elemento, this->cores_borda[RADIO_BASE]);

      Lista_t.insert(this->elementos[RADIO_BASE], (Item) new_elemento);
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

        this->max_width_qry  = max(this->max_width_qry, x + w + 4);
        this->max_height_qry = max(this->max_height_qry, y + h + 4);

        strcpy(saida, "q?:\n");
      } else {
        r = strtof(params[0], NULL);
        x = strtof(params[1], NULL);
        y = strtof(params[2], NULL);

        figAtual = cria_circulo(x, y, r, "transparent", "black");

        this->max_width_qry  = max(this->max_width_qry, x + r + 4);
        this->max_height_qry = max(this->max_height_qry, y + r + 4);

        strcpy(saida, "Q?:\n");
      }

      Lista_t.insert(this->saida, (Item) saida);

      for (i = 0; i < 4; i++) {
        Lista lista_atual = this->elementos[i];

        Posic iterator = Lista_t.get_first(lista_atual);

        iterator = Lista_t.search(
          lista_atual, iterator, figAtual, elemento_dentro_figura);

        while (iterator) {
          new_elemento = (Elemento) Lista_t.get(lista_atual, iterator);

          saida = get_info_elemento(new_elemento);
          strcat(saida, "\n");
          Lista_t.insert(this->saida, (Item) saida);

          iterator = Lista_t.search(
            lista_atual,
            Lista_t.get_next(lista_atual, iterator),
            figAtual,
            elemento_dentro_figura);
        }
      }

      Lista_t.insert(this->saida_svg_qry, (Item) figAtual);

      break;
    case QRY_DELETE_QUADRA_RECT:
    case QRY_DELETE_QUADRA_CIRC:
      if (tipo == QRY_DELETE_QUADRA_RECT) {
        x = strtof(params[0], NULL);
        y = strtof(params[1], NULL);
        w = strtof(params[2], NULL);
        h = strtof(params[3], NULL);

        figAtual = cria_retangulo(x, y, w, h, "transparent", "black");

        this->max_width_qry  = max(this->max_width_qry, x + w + 4);
        this->max_height_qry = max(this->max_height_qry, y + h + 4);
      } else {
        r = strtof(params[0], NULL);
        x = strtof(params[1], NULL);
        y = strtof(params[2], NULL);

        figAtual = cria_circulo(x, y, r, "transparent", "black");

        this->max_width_qry  = max(this->max_width_qry, x + r + 4);
        this->max_height_qry = max(this->max_height_qry, y + r + 4);
      }

      Lista lista_atual = this->elementos[QUADRA];
      iterator          = Lista_t.get_first(lista_atual);
      Posic next_it;

      iterator = Lista_t.search(
        lista_atual, iterator, figAtual, elemento_dentro_figura);

      while (iterator) {
        next_it = Lista_t.get_next(lista_atual, iterator);

        new_elemento = Lista_t.get(lista_atual, iterator);

        Lista_t.remove(lista_atual, iterator);

        cep   = get_cep_elemento(new_elemento);
        saida = calloc(20 + strlen(cep), sizeof(char));
        sprintf(saida, "Quadra: %s deletada.\n", cep);  // 20 Caracteres
        Lista_t.insert(this->saida, (Item) saida);

        destruir_elemento(new_elemento);

        iterator = Lista_t.search(
          lista_atual, next_it, figAtual, elemento_dentro_figura);
      }

      Lista_t.insert(this->saida_svg_qry, (Item) figAtual);

      break;
    case QRY_DELETE_ALL_RECT:
    case QRY_DELETE_ALL_CIRC:
      if (tipo == QRY_DELETE_ALL_RECT) {
        x = strtof(params[1], NULL);
        y = strtof(params[2], NULL);
        w = strtof(params[3], NULL);
        h = strtof(params[4], NULL);

        figAtual = cria_retangulo(x, y, w, h, "transparent", "black");

        this->max_width_qry  = max(this->max_width_qry, x + w + 4);
        this->max_height_qry = max(this->max_height_qry, y + h + 4);
      } else {
        x = strtof(params[1], NULL);
        y = strtof(params[2], NULL);
        r = strtof(params[3], NULL);

        figAtual = cria_circulo(x, y, r, "transparent", "black");

        this->max_width_qry  = max(this->max_width_qry, x + r + 4);
        this->max_height_qry = max(this->max_height_qry, y + r + 4);
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
        Posic iterator    = Lista_t.get_first(lista_atual);
        Posic next_it;

        iterator = Lista_t.search(
          lista_atual, iterator, figAtual, elemento_dentro_figura);

        while (iterator) {
          next_it = Lista_t.get_next(lista_atual, iterator);

          new_elemento = Lista_t.get(lista_atual, iterator);

          Lista_t.remove(lista_atual, iterator);

          sufixo = get_info_elemento(new_elemento);
          saida  = calloc(12 + strlen(sufixo), sizeof(char));
          sprintf(saida, "%s deletada.\n", sufixo);
          Lista_t.insert(this->saida, (Item) saida);
          free(sufixo);

          destruir_elemento(new_elemento);

          iterator = Lista_t.search(
            lista_atual, next_it, figAtual, elemento_dentro_figura);
        }
      }

      Lista_t.insert(this->saida_svg_qry, (Item) figAtual);

      free(tipos_pesquisa);

      break;
    case QRY_MUDA_COR_QUADRA:
      cep       = params[0];
      cor_borda = params[1];
      cor       = params[2];

      for (int i = 0; i < 4; i++) {
        Lista lista_atual = this->elementos[i];

        Posic posic_elemento = Lista_t.get_first(lista_atual);
        posic_elemento =
          Lista_t.search(lista_atual, posic_elemento, cep, checar_id_elemento);

        if (!posic_elemento)
          continue;

        new_elemento = Lista_t.get(lista_atual, posic_elemento);

        set_cor_elemento(new_elemento, cor);
        set_cor_borda_elemento(new_elemento, cor_borda);
      }

      break;
    case QRY_PRINT_EQUIPAMENTO:
      cep = params[0];

      for (int i = 0; i < 4; i++) {
        Lista lista_atual = this->elementos[i];

        Posic posic_elemento = Lista_t.get_first(lista_atual);
        posic_elemento =
          Lista_t.search(lista_atual, posic_elemento, cep, checar_id_elemento);

        if (!posic_elemento)
          continue;

        new_elemento = Lista_t.get(lista_atual, posic_elemento);
        saida        = get_info_elemento(new_elemento);
        strcat(saida, "\n");
        Lista_t.insert(this->saida, saida);
      }

      break;
    case QRY_CHECA_RADIO_BASE_PROXIMA:;
      int tam = Lista_t.length(this->elementos[RADIO_BASE]);

      // Quer dizer que nao há Tores de Celular
      if (tam <= 1) {
        length = 63;
        saida  = calloc(length, sizeof(char));
        sprintf(
          saida,
          "Nao ha torres de celular suficientes para checar a distancia.\n");
        Lista_t.insert(this->saida, saida);

        break;
      }

      Elemento *radios_base = Lista_t.to_array(this->elementos[RADIO_BASE]);

      ClosestPair pair = closest_pair(radios_base, tam);

      Elemento radio1, radio2;

      radio1    = pair.point1;
      radio2    = pair.point2;
      distancia = pair.dist;

      // Reportar as id das torres
      cor = get_id_elemento(radio1);
      cep = get_id_elemento(radio2);

      length = 49 + strlen(cor) + strlen(cep) + 6;

      saida = (char *) calloc(length, sizeof(char));

      sprintf(
        saida,
        "Torres de radio mais proximas:\n%s e %s. Distancia: %3.2f\n",
        cor,
        cep,
        distancia);

      Lista_t.insert(this->saida, saida);

      x = get_x(radio1);
      y = get_y(radio1);
      Lista_t.insert(
        this->saida_svg_qry,
        cria_circulo(x, y, RAIO_RADIOS_PROXIMOS, "transparent", "purple"));

      this->max_width_qry = max(this->max_width_qry, x + RAIO_RADIOS_PROXIMOS);
      this->max_height_qry =
        max(this->max_height_qry, y + RAIO_RADIOS_PROXIMOS);

      x = get_x(radio2);
      y = get_y(radio2);
      Lista_t.insert(
        this->saida_svg_qry,
        cria_circulo(x, y, RAIO_RADIOS_PROXIMOS, "transparent", "purple"));

      this->max_width_qry = max(this->max_width_qry, x + RAIO_RADIOS_PROXIMOS);
      this->max_height_qry =
        max(this->max_height_qry, y + RAIO_RADIOS_PROXIMOS);

      free(radios_base);
      break;

    case COMENTARIO:
    case NONE: break;
  }

  destruir_comando(com);

  return 1;
}

int ha_comandos(Controlador c) {
  struct Controlador *this = (struct Controlador *) c;

  return !!Lista_t.length(this->fila_execucao);
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

    Lista_t.insert(this->fila_execucao, (Item) comando);

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
    Lista_t.insert(this->fila_execucao, (Item) cria_comando(linha));

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
  Posic iterator;

  // Escreve o txt do .geo
  escrever_txt_final(c);

  char *qry_file, *geo_file;

  if (!this->arq_query)
    return;

  qry_file = get_nome(this->arq_query);
  geo_file = get_nome(this->nome_base);

  // Arquivo [nome_base]-[nome_qry].svg
  SVG s;

  length    = 7 + strlen(this->dir_saida) + strlen(qry_file) + strlen(geo_file);
  full_path = calloc(length, sizeof(char));
  sprintf(full_path, "%s%s-%s.svg", this->dir_saida, geo_file, qry_file);

  s = cria_SVG(full_path, this->max_width_qry, this->max_height_qry);

  desenhar_elementos(c, s);

  iterator = Lista_t.get_first(this->saida_svg_qry);
  while (iterator) {
    desenha_figura(
      s, Lista_t.get(this->saida_svg_qry, iterator), 0.8, SVG_BORDA_TRACEJADA);
    iterator = Lista_t.get_next(this->saida_svg_qry, iterator);
  }

  salva_SVG(s);

  free(full_path);
  free(qry_file);
  free(geo_file);

  destruir_SVG(s);
}

void destruir_controlador(Controlador c) {
  struct Controlador *this;
  int i;

  this = (struct Controlador *) c;

  Lista_t.destruir(this->saida, &free);
  Lista_t.destruir(this->saida_svg_qry, &destruir_figura);

  Lista_t.destruir(this->figuras, &destruir_figura);

  // Sobreposicoes
  Lista_t.destruir(this->sobreposicoes, &destruir_figura);

  // Elementos
  Lista_t.destruir(this->elementos[QUADRA], &destruir_elemento);
  Lista_t.destruir(this->elementos[HIDRANTE], &destruir_elemento);
  Lista_t.destruir(this->elementos[SEMAFORO], &destruir_elemento);
  Lista_t.destruir(this->elementos[RADIO_BASE], &destruir_elemento);

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

  Lista_t.destruir(this->fila_execucao, NULL);

  free(c);
}

char *get_nome_base(Controlador c) {
  return ((struct Controlador *) c)->nome_base;
}

char *get_nome_query(Controlador c) {
  return ((struct Controlador *) c)->arq_query;
}

/** METODOS PRIVADOS */

static void desenhar_todas_figuras(Controlador c, SVG s) {
  struct Controlador *this;

  Figura figAtual;

  this = (struct Controlador *) c;

  Posic iterator = Lista_t.get_first(this->figuras);

  while (iterator) {
    figAtual = Lista_t.get(this->figuras, iterator);
    desenha_figura(s, figAtual, 0.4, SVG_BORDA_SOLIDA);
    iterator = Lista_t.get_next(this->figuras, iterator);
  }
}

static void desenhar_sobreposicoes(Controlador c, SVG s) {
  struct Controlador *this;
  Figura figDash;

  this = (struct Controlador *) c;

  if (!Lista_t.length(this->sobreposicoes))
    return;

  /* Calcular retangulo das sobreposicoes */
  Posic iterator = Lista_t.get_first(this->sobreposicoes);

  while (iterator) {
    figDash = (Figura) Lista_t.get(this->sobreposicoes, iterator);

    desenha_figura(s, figDash, 1.0, SVG_BORDA_TRACEJADA);
    escreve_texto(
      s, "sobrepoe", get_x(figDash), get_y(figDash) - 5, 15, "purple");

    iterator = Lista_t.get_next(this->sobreposicoes, iterator);
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

  arq = abrir_arquivo(full_path, ALTERACAO);

  free(full_path);

  Posic iterator = Lista_t.get_first(this->saida);

  while (iterator) {
    escrever_linha(arq, (char *) Lista_t.get(this->saida, iterator));
    iterator = Lista_t.get_next(this->saida, iterator);
  }

  fechar_arquivo(arq);
}

static void desenhar_elementos(Controlador _this, SVG svg) {
  struct Controlador *this = (struct Controlador *) _this;
  int i;
  Lista lista_atual;
  Posic iterator;
  Elemento elemento_atual;

  // Para cada tipo de elemento
  for (i = 0; i < 4; i++) {
    lista_atual = this->elementos[i];

    // Itera pela lista e desesenha no SVG os elementos
    iterator = Lista_t.get_first(lista_atual);
    while (iterator) {
      elemento_atual = (Elemento) Lista_t.get(lista_atual, iterator);

      desenha_elemento(svg, elemento_atual);

      iterator = Lista_t.get_next(lista_atual, iterator);
    }
  }
}

static int checar_id_figura(const Item _figura, const void *_id) {
  const Figura figura = (const Figura) _figura;
  const int id        = (const int) *(int *) _id;

  return !(get_id_figura(figura) == id);
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

static int checar_id_elemento(const Item _elemento, const void *_id) {
  const Elemento elemento = (const Elemento) _elemento;
  const char *id          = (const char *) _id;

  char *id_elemento = get_id_elemento(elemento);

  return strcmp(id_elemento, id);
}