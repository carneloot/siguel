#ifndef __CONTROLADOR_R__
#define __CONTROLADOR_R__

#include <model/modules/lista.h>
#include <model/modules/kdtree.h>
#include <model/modules/hash.h>
#include <model/modules/ponto2d.h>
#include <model/modules/grafod.h>

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
  Lista saida;
  Lista saida_svg_qry;

  char *nome_base;
  char *dir_saida;
  char *dir_entrada;

  char *extras[EXTRAS_TOTAL];

  Lista comercios;
  Lista pessoas;
  HashTable tabelas[TABELAS_TOTAL];

  Lista veiculos;
  
  Lista figuras;

  int linha_atual;

  Lista sobreposicoes;

  KDTree elementos[4];
  char *cores[4];
  char *cores_borda[4];

  Ponto2D max_geo, max_qry;

  Lista fila_execucao;

  // O valor padrão dos registradores é (0, 0)
  Ponto2D registradores[11];

  GrafoD mapa_viario;
  KDTree vertices_mapa_viario;
  Lista arestas_mapa_viario;
};

extern void desenhar_todas_figuras(void *controlador, void *svg);

extern void desenhar_sobreposicoes(void *controlador, void *svg);

extern void desenhar_elementos(void *controlador, void *svg);

extern void desenhar_mapa_viario(void *controlador, void *svg);

extern void desenhar_veiculos(void *this, void *svg);

#endif /* __CONTROLADOR_R__ */