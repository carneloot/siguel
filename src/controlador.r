#ifndef __CONTROLADOR_R__
#define __CONTROLADOR_R__

#include <model/colecao/lista.h>
#include <model/colecao/kdtree.h>
#include <model/colecao/hash.h>
#include <model/colecao/grafod.h>

#include <model/estrutural/ponto2d.h>

#define LISTA_EXTRAS \
  X( ec, "-ec") \
  X( pm, "-pm") \
  X(via,  "-v") \
  X(qry,  "-q")

enum TipoExtras {
  #define X(a, b) e_##a,
  LISTA_EXTRAS
  #undef X
  EXTRAS_TOTAL
};

enum Tabelas {
  TIPO_X_DESCRICAO,
  CNPJ_X_COMERCIO,
  CPF_X_PESSOA,
  CPF_X_CEP,
  CEP_X_QUADRA,
  ID_X_RADIO,
  ID_X_HIDRANTE,
  ID_X_SEMAFORO,
  TABELAS_TOTAL
};

struct Controlador {
  Lista_t saida;
  Lista_t saida_svg_qry;

  char *nome_base;
  char *dir_saida;
  char *dir_entrada;

  char *extras[EXTRAS_TOTAL];

  Lista_t comercios;
  Lista_t pessoas;
  HashTable_t tabelas[TABELAS_TOTAL];

  Lista_t veiculos;

  Lista_t colisoes;
  
  Lista_t figuras;

  int linha_atual;

  Lista_t sobreposicoes;

  KDTree_t elementos[4];
  char *cores[4];
  char *cores_borda[4];

  Ponto2D_t max_geo, max_qry;

  Lista_t fila_execucao;

  // O valor padrão dos registradores é (0, 0)
  Ponto2D_t registradores[11];

  GrafoD_t mapa_viario;
  KDTree_t vertices_mapa_viario;
  KDTree_t arestas_mapa_viario;
};

extern void desenhar_todas_figuras(void *controlador, void *svg);

extern void desenhar_sobreposicoes(void *controlador, void *svg);

extern void desenhar_elementos(void *controlador, void *svg);

extern void desenhar_mapa_viario(void *controlador, void *svg);

extern void desenhar_veiculos(void *this, void *svg);

#endif /* __CONTROLADOR_R__ */