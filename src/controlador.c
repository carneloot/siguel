#include "controlador.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SVG.h"
#include "elemento.h"
#include "figura.h"
#include "modules/logger.h"
#include "utils.h"
#include <comercio/tipo_comercio.h>

#include "controlador.r"

const char *extra_extensao[EXTRAS_TOTAL] = {
  #define X(a, b) [e_##a] = #a,
  LISTA_EXTRAS
  #undef X
};

const char *extra_flag[EXTRAS_TOTAL] = {
  #define X(a, b) [e_##a] = b,
  LISTA_EXTRAS
  #undef X
};

static void escrever_txt_final(void *c);

/** ===== FUNCOES DA ARVORE ===== */

int equalElements(const void *_a, const void *_b) {
  const Elemento a = (const Elemento) _a;
  const Elemento b = (const Elemento) _b;

  return (Ponto2D_t.equal(get_pos(a), get_pos(b)) && !strcmp(get_id_elemento(a), get_id_elemento(b)));
}

int compareX(const void *_a, const void *_b) {
  Elemento a = (Elemento) _a;
  Elemento b = (Elemento) _b;

  Ponto2D posA = get_pos(a);
  Ponto2D posB = get_pos(b);

  return (posA.x - posB.x);
}

int compareY(const void *_a, const void *_b) {
  Elemento a = (Elemento) _a;
  Elemento b = (Elemento) _b;

  Ponto2D posA = get_pos(a);
  Ponto2D posB = get_pos(b);

  return (posA.y - posB.y);
}

void desenharElementoSVG(const Item _ele, unsigned prof, va_list _list) {
  va_list list;
  va_copy(list, _list);
  const Elemento ele = (const Elemento) _ele;

  SVG svg = va_arg(list, SVG);
  
  desenha_elemento(svg, ele);
}

int elementoDentro(Item _this, int dim, Item rect[]) {
  Elemento this = (Elemento) _this;

  int result;

  Ponto2D pos = get_pos(this);
  Ponto2D *a  = (Ponto2D *) rect[0];
  Ponto2D *b  = (Ponto2D *) rect[1];

  result = 0;

  switch (dim) {
    case -1:
      result =
        !(pos.x < a->x || pos.x > b->x ||  //
          pos.y < a->y || pos.y > b->y);
      break;
    case 0:
      if (pos.x < a->x)
        result = -1;
      else if (pos.x > b->x)
        result = 1;
      break;
    case 1:
      if (pos.y < a->y)
        result = -1;
      else if (pos.y > b->y)
        result = 1;
      break;
  }

  return result;
}

/** METODOS PUBLICOS */

Controlador cria_controlador() {
  int i;
  struct Controlador *this =
    (struct Controlador *) malloc(sizeof(struct Controlador));

  this->saida         = Lista_t.create();
  this->saida_svg_qry = Lista_t.create();

  this->nome_base   = NULL;
  this->dir_saida   = NULL;

  this->dir_entrada = (char *) malloc(3 * sizeof(char));
  strcpy(this->dir_entrada, "./");

  for (i = 0; i < EXTRAS_TOTAL; i++)
    this->extras[i] = NULL;

  this->figuras = Lista_t.create();

  this->linha_atual = 0;

  this->sobreposicoes = Lista_t.create();

  for (i = 0; i < 4; i++) {
    this->elementos[i] = KDTree_t.create(2, equalElements, compareX, compareY);

    this->cores[i]       = NULL;
    this->cores_borda[i] = NULL;
  }

  this->max_geo = Ponto2D_t.new(0, 0);
  this->max_qry = Ponto2D_t.new(0, 0);

  this->fila_execucao = Lista_t.create();

  this->tipos_comercios = HashTable_t.create(199);

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

    else  {
      for (int k = 0; k < EXTRAS_TOTAL; k++) {
        if (!strcmp(argv[i], extra_flag[k])) {
          i++;
          this->extras[k] = remover_extensao(argv[i]);
          break;
        }
      }
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

  LOG_PRINT(LOG_FILE, "Arquivo \"%s.geo\" iniciado.", this->nome_base);
  for (i = 0; i < EXTRAS_TOTAL; i++) {
    if (!this->extras[i]) continue;
    LOG_PRINT(LOG_FILE, "Arquivo \"%s.%s\" iniciado.", this->extras[i], extra_extensao[i]);
  }
}

int executar_proximo_comando(Controlador c) {
  struct Controlador *this = (struct Controlador *) c;
  Comando comando;

  Posic inicio_lista = Lista_t.get_first(this->fila_execucao);

  comando = (Comando) Lista_t.get(this->fila_execucao, inicio_lista);
  Lista_t.remove(this->fila_execucao, inicio_lista);

  this->linha_atual++;

  int result = comando->executar(comando, c);
  
  destruir_comando(comando);

  return result;
}

int ha_comandos(Controlador c) {
  struct Controlador *this = (struct Controlador *) c;

  return !!Lista_t.length(this->fila_execucao);
}

void gerar_fila_execucao(Controlador _this) {
  struct Controlador *this = (struct Controlador *) _this;

  // Adicionar os arquivos a serem verificados na lista
  Lista arquivos = Lista_t.create();

  size_t length;
  char *path;

  length = strlen(this->nome_base) + strlen(this->dir_entrada) + 6;
  path   = (char *) malloc(length * sizeof(char));
  sprintf(path, "%s/%s.geo", this->dir_entrada, this->nome_base);
  Lista_t.insert(arquivos, path);

  // Se o arquivo de qry foi especificado
  for (int i = 0; i < EXTRAS_TOTAL; i++) {
    if (!this->extras[i]) continue;

    length = strlen(this->extras[i]) + strlen(this->dir_entrada) + 3 + strlen(extra_extensao[i]);
    path   = (char *) malloc(length * sizeof(char));
    sprintf(path, "%s/%s.%s", this->dir_entrada, this->extras[i], extra_extensao[i]);
    Lista_t.insert(arquivos, path);
  }

  // Adicionando comandos a fila de execução
  Posic iterator = Lista_t.get_first(arquivos);

  while (iterator) {
    path        = Lista_t.get(arquivos, iterator);
    Arquivo arq = abrir_arquivo(path, LEITURA);

    char *linha;
    char *extensao = get_extensao(path);

    while ((linha = ler_proxima_linha(arq))) {

      Comando comando = cria_comando(linha, extensao);

      // Comentarios nao sao inseridos na fila de execução
      if (!comando) {
        char *codigo_comando = strtok(linha, " ");
        LOG_PRINT(LOG_STDOUT, "ERRO: Comando inexistente \"%s\"", codigo_comando);

        free(linha);
        continue;
      }
      
      LOG_PRINT(LOG_FILE, "Comando inserido: \"%s\"", linha);

      Lista_t.insert(this->fila_execucao, (Item) comando);

      free(linha);
    }

    free(extensao);

    fechar_arquivo(arq);

    iterator = Lista_t.get_next(arquivos, iterator);
  }

  Lista_t.destruir(arquivos, free);
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

  if (!this->extras[e_qry])
    return;

  qry_file = get_nome(this->extras[e_qry]);
  geo_file = get_nome(this->nome_base);

  // Arquivo [nome_base]-[nome_qry].svg
  SVG s;

  length    = 7 + strlen(this->dir_saida) + strlen(qry_file) + strlen(geo_file);
  full_path = calloc(length, sizeof(char));
  sprintf(full_path, "%s%s-%s.svg", this->dir_saida, geo_file, qry_file);

  s = cria_SVG(full_path, this->max_qry.x, this->max_qry.y);

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
  for (i = 0; i < 4; i++) {
    KDTree_t.destroy(this->elementos[i], &destruir_elemento);
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
  
  for (i = 0; i < EXTRAS_TOTAL; i++)
    if (this->extras[i])
      free(this->extras[i]);

  Lista_t.destruir(this->fila_execucao, NULL);

  HashTable_t.destroy(this->tipos_comercios, tipo_comercio_destruir, 1);

  free(c);
}

char *get_nome_base(Controlador c) {
  return ((struct Controlador *) c)->nome_base;
}

char *get_nome_query(Controlador c) {
  return ((struct Controlador *) c)->extras[e_qry];
}

/** METODOS PRIVADOS */

void desenhar_todas_figuras(void *c, void *s) {
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

void desenhar_sobreposicoes(void *c, void *s) {
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
    Ponto2D pos = get_pos(figDash);
    pos.y -= 5;
    escreve_texto(s, "sobrepoe", pos, 15, "purple");

    iterator = Lista_t.get_next(this->sobreposicoes, iterator);
  }
}

static void escrever_txt_final(void *c) {
  struct Controlador *this;
  size_t tamanho_total;
  char *full_path;
  Arquivo arq;

  this = (struct Controlador *) c;

  if (!this->nome_base)
    return;
    
  LOG_PRINT(LOG_FILE, "Escrevendo txt final.");

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

void desenhar_elementos(void *_this, void *svg) {
  struct Controlador *this = (struct Controlador *) _this;
  
  LOG_PRINT(LOG_FILE, "Desenhando elementos.");

  for (int i = 0; i < 4; i++) {
    KDTree arvore_atual = this->elementos[i];
    
    if (KDTree_t.is_empty(arvore_atual))
      continue;

    KDTree_t.passe_simetrico(arvore_atual, desenharElementoSVG, svg);
  }
}
