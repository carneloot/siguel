#include "controlador.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <string.h>

#include <SVG.h>
#include <elemento.h>
#include <desenhavel.h>
#include <figura.h>
#include <comercio.h>
#include <pessoa.h>
#include <utils.h>
#include <modules/logger.h>
#include <veiculo.h>
#include <colisao.h>

#include <mapa_viario/aresta.h>
#include <mapa_viario/vertice.h>

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

  return (p2d_equal(get_pos(a), get_pos(b)) && !strcmp(get_id_elemento(a), get_id_elemento(b)));
}

int compareX(const void *_a, const void *_b) {
  Elemento a = (Elemento) _a;
  Elemento b = (Elemento) _b;

  Ponto2D_t posA = get_pos(a);
  Ponto2D_t posB = get_pos(b);

  return (posA.x - posB.x);
}

int compareY(const void *_a, const void *_b) {
  Elemento a = (Elemento) _a;
  Elemento b = (Elemento) _b;

  Ponto2D_t posA = get_pos(a);
  Ponto2D_t posB = get_pos(b);

  return (posA.y - posB.y);
}

void desenharElementoSVG(const Item _ele, unsigned prof, va_list list) {
  const Elemento ele = (const Elemento) _ele;
  SVG svg = va_arg(list, SVG);
  
  desenha_elemento(svg, ele);
}

int elementoDentro(Item _this, int dim, Item rect[]) {
  Elemento this = (Elemento) _this;

  int result;

  Ponto2D_t pos = get_pos(this);
  Ponto2D_t *a  = (Ponto2D_t *) rect[0];
  Ponto2D_t *b  = (Ponto2D_t *) rect[1];

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

/** FUNCOES ARVORE DO GRAFO */

int equalVertices(const void *_a, const void *_b) {
  const VerticeInfo a = (const VerticeInfo) _a;
  const VerticeInfo b = (const VerticeInfo) _b;

  return (p2d_equal(a->pos, b->pos) && !strcmp(a->id, b->id));
}

int compareXVertice(const void *_a, const void *_b) {
  const VerticeInfo a = (const VerticeInfo) _a;
  const VerticeInfo b = (const VerticeInfo) _b;

  return (a->pos.x - b->pos.x);
}

int compareYVertice(const void *_a, const void *_b) {
  const VerticeInfo a = (const VerticeInfo) _a;
  const VerticeInfo b = (const VerticeInfo) _b;

  return (a->pos.y - b->pos.y);
}

int equalArestas( const void* _this, const void* _other ) {
  ArestaInfo this  = (ArestaInfo) _this;
  ArestaInfo other = (ArestaInfo) _other;
  return (!strcmp(this->nome, other->nome) && p2d_equal(this->pos, other->pos));
}

int compareXAresta( const void* _this, const void* _other ) {
  ArestaInfo this  = (ArestaInfo) _this;
  ArestaInfo other = (ArestaInfo) _other;
  return ( this->pos.x - other->pos.x );
}

int compareYAresta( const void* _this, const void* _other ) {
  ArestaInfo this  = (ArestaInfo) _this;
  ArestaInfo other = (ArestaInfo) _other;
  return ( this->pos.y - other->pos.y );
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

  this->veiculos = Lista_t.create();
  this->colisoes = Lista_t.create();
  this->figuras = Lista_t.create();

  this->linha_atual = 0;

  this->sobreposicoes = Lista_t.create();

  for (i = 0; i < 4; i++) {
    this->elementos[i] = KDTree_t.create(2, equalElements, compareX, compareY);

    this->cores[i]       = NULL;
    this->cores_borda[i] = NULL;
  }

  this->max_geo = p2d_new(0, 0);
  this->max_qry = p2d_new(0, 0);

  this->fila_execucao = Lista_t.create();

  this->comercios = Lista_t.create();
  this->pessoas   = Lista_t.create();

  for (i = 0; i < TABELAS_TOTAL; i++)
    this->tabelas[i] = HashTable_t.create(73);

  for (i = 0; i < 11; i++) {
    this->registradores[i] = p2d_new(0, 0);
  }

  this->mapa_viario          = GrafoD_t.create();
  this->vertices_mapa_viario = KDTree_t.create(2, equalVertices, compareXVertice, compareYVertice);
  this->arestas_mapa_viario  = KDTree_t.create(2, equalArestas, compareXAresta, compareYAresta);

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

  LOG_PRINT(LOG_FILE, "Executando comando \"%s\".", comando->string);

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

  char *path = format_string(
    "%s%s.geo", this->dir_entrada, this->nome_base);
  Lista_t.insert(arquivos, path);

  // Se o arquivo de qry foi especificado
  for (int i = 0; i < EXTRAS_TOTAL; i++) {
    if (!this->extras[i]) continue;

    path = format_string(
      "%s%s.%s", this->dir_entrada, this->extras[i], extra_extensao[i]);
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

  full_path = format_string(
    "%s%s-%s.svg", this->dir_saida, geo_file, qry_file);

  s = cria_SVG(full_path, this->max_qry.x, this->max_qry.y);

  desenhar_elementos(c, s);

  if (this->extras[e_via])
    desenhar_mapa_viario(this, s);

  desenhar_veiculos(c, s);

  // Desenhando saida dos comandos

  iterator = Lista_t.get_first(this->saida_svg_qry);
  while (iterator) {
    desenha_desenhavel(s, Lista_t.get(this->saida_svg_qry, iterator));
    iterator = Lista_t.get_next(this->saida_svg_qry, iterator);
  }

  salva_SVG(s);

  free(full_path);
  free(qry_file);
  free(geo_file);

  destruir_SVG(s);
}

void printar_mensagem_final(Controlador _this, int eh_erro) {
  struct Controlador * this = (struct Controlador *) _this;

  if (eh_erro) {
    LOG_PRINT(
      LOG_STDOUT, "Arquivo \"%s.geo\" finalizado com erro.", this->nome_base);
    return;
  }

  LOG_PRINT(LOG_FILE, "Arquivo \"%s.geo\" finalizado.", this->nome_base);

  int count = 0;

  for (int i = 0; i < EXTRAS_TOTAL; i++)
    count += !!this->extras[i];

  if (!count) {
    printf("Arquivo \"%s.geo\" finalizado com sucesso.\n", this->nome_base);
    return;
  }

  printf("Arquivos \"%s.geo\"", this->nome_base);
  for (int i = 0; i < EXTRAS_TOTAL; i++) {
    if (!this->extras[i]) continue;
    printf(" \"%s.%s\"", this->extras[i], extra_extensao[i]);
  }
  printf(" finalizados com sucesso.\n");

}

HashTable get_table_quadras(Controlador _this) {
  struct Controlador * this = (struct Controlador *) _this;
  return this->tabelas[CEP_X_QUADRA];
}

void destruir_controlador(Controlador c) {
  struct Controlador *this;
  int i;

  this = (struct Controlador *) c;

  Lista_t.destruir(this->saida, &free);
  Lista_t.destruir(this->saida_svg_qry, desenhavel_destruir);

  Lista_t.destruir(this->veiculos, destruir_veiculo);
  Lista_t.destruir(this->colisoes, colisao_destroy);
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

  Lista_t.destruir(this->comercios, comercio_destruir);
  Lista_t.destruir(this->pessoas,     pessoa_destruir);

  HashTable_t.destroy(this->tabelas[CPF_X_CEP],        NULL, 0);
  HashTable_t.destroy(this->tabelas[CEP_X_QUADRA],     NULL, 0);
  HashTable_t.destroy(this->tabelas[TIPO_X_DESCRICAO], free, 1);
  HashTable_t.destroy(this->tabelas[CPF_X_PESSOA],     NULL, 0);
  HashTable_t.destroy(this->tabelas[CNPJ_X_COMERCIO],  NULL, 0);
  HashTable_t.destroy(this->tabelas[ID_X_RADIO],       NULL, 0);
  HashTable_t.destroy(this->tabelas[ID_X_HIDRANTE],    NULL, 0);
  HashTable_t.destroy(this->tabelas[ID_X_SEMAFORO],    NULL, 0);

  GrafoD_t.destroy(this->mapa_viario);
  KDTree_t.destroy(this->vertices_mapa_viario, destroy_vertice_info);
  KDTree_t.destroy(this->arestas_mapa_viario,  destroy_aresta_info);

  free(c);
}

/** METODOS PRIVADOS */

static void escrever_txt_final(void *c) {
  struct Controlador *this;
  char *full_path;
  Arquivo arq;

  this = (struct Controlador *) c;

  if (!this->nome_base)
    return;
    
  LOG_PRINT(LOG_FILE, "Escrevendo txt final.");

  full_path = format_string(
    "%s%s.txt", this->dir_saida, this->nome_base);

  arq = abrir_arquivo(full_path, ALTERACAO);

  free(full_path);

  if (this->extras[e_qry] == NULL)
    return;

  char *nome_qry = get_nome(this->extras[e_qry]);

  escrever_linha(arq, "Arquivo \"%s.qry\":\n\n", nome_qry);

  free(nome_qry);

  Posic iterator = Lista_t.get_first(this->saida);

  while (iterator) {
    escrever_linha(arq, (char *) Lista_t.get(this->saida, iterator));
    iterator = Lista_t.get_next(this->saida, iterator);
  }

  escrever_linha(arq, "\n\n");

  fechar_arquivo(arq);
}

void desenhar_todas_figuras(void *c, void *s) {
  struct Controlador *this;

  Figura figAtual;

  this = (struct Controlador *) c;

  Posic iterator = Lista_t.get_first(this->figuras);

  while (iterator) {
    figAtual = Lista_t.get(this->figuras, iterator);
    desenha_figura(s, figAtual, 0.4, FIG_BORDA_SOLIDA);
    iterator = Lista_t.get_next(this->figuras, iterator);
  }
}

void desenhar_sobreposicoes(void *c, void *s) {
  struct Controlador *this;
  Figura figDash;

  this = (struct Controlador *) c;

  if (!Lista_t.length(this->sobreposicoes))
    return;

  escreve_comentario(s, "INICIO SOBREPOSICOES");

  /* Calcular retangulo das sobreposicoes */
  Posic iterator = Lista_t.get_first(this->sobreposicoes);

  while (iterator) {
    figDash = (Figura) Lista_t.get(this->sobreposicoes, iterator);

    desenha_figura(s, figDash, 1.0, FIG_BORDA_TRACEJADA);
    Ponto2D_t pos = get_pos(figDash);
    pos.y -= 5;
    escreve_texto(s, "sobrepoe", pos, 15, "purple");

    iterator = Lista_t.get_next(this->sobreposicoes, iterator);
  }

  escreve_comentario(s, "FIM SOBREPOSICOES");
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

void desenha_veiculo(const Item _veiculo, const void *_svg) {
  Veiculo veiculo = _veiculo;
  const SVG svg = (const SVG) _svg;

  escreve_comentario(svg, "VEICULO: %s", get_placa_veiculo(veiculo));

  Desenhavel desenhavel = cria_desenhavel(veiculo, get_svg_veiculo, NULL);

  desenha_desenhavel(svg, desenhavel);

  desenhavel_destruir(desenhavel);
}
  
void desenhar_veiculos(void *_this, void *svg) {
  struct Controlador* this = _this;

  Lista_t.map(this->veiculos, svg, desenha_veiculo);

}

void desenhar_vertice(const Item _vertice, unsigned profundidade, va_list list) {
  VerticeInfo vertice = (VerticeInfo) _vertice;
  SVG svg = va_arg(list, SVG);

  Figura fig_vertice = cria_circulo(vertice->pos.x, vertice->pos.y, 5, "black", "transparent");

  escreve_comentario(svg, "VERTICE: \"%s\"", vertice->id);
  
  desenha_figura(svg, fig_vertice, 0.6, FIG_BORDA_SOLIDA);

  destruir_figura(fig_vertice);
}

void desenhar_aresta(const Item _aresta, unsigned profundidade, va_list list) {
  ArestaInfo aresta  = (ArestaInfo) _aresta;
  SVG svg            = va_arg(list, SVG);
  GrafoD mapa_viario = va_arg(list, GrafoD);

  VerticeInfo origem  = GrafoD_t.get_info_vertice(mapa_viario, aresta->origem);
  VerticeInfo destino = GrafoD_t.get_info_vertice(mapa_viario, aresta->destino);

  escreve_comentario(svg, "ARESTA \"%s\" -> \"%s\"", origem->id, destino->id);

  if (!get_aresta_valido(aresta)) {
    desenha_linha(svg, origem->pos, aresta->pos, 0.6, 3, "red", true);
    desenha_linha(svg, aresta->pos, destino->pos, 0.6, 3, "red", false);
  } else {
    desenha_linha(svg, origem->pos, aresta->pos, 0.6, 3, "black", true);
    desenha_linha(svg, aresta->pos, destino->pos, 0.6, 3, "black", false);
  }

}

void desenhar_mapa_viario(void *_this, void *svg) {
  #ifdef DEBUG

  struct Controlador *this = (struct Controlador *) _this;
  
  LOG_PRINT(LOG_FILE, "Desenhando mapa viario.");

  KDTree_t.passe_simetrico(this->vertices_mapa_viario, desenhar_vertice, svg);

  KDTree_t.passe_simetrico(this->arestas_mapa_viario, desenhar_aresta, svg, this->mapa_viario);

  #endif
}
