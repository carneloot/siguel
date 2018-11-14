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
  CEP_X_QUADRA,
  CPF_X_PESSOA,
  CPF_X_CEP,
  CNPJ_X_COMERCIO,
  ID_X_RADIO,
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
};

extern void desenhar_todas_figuras(void *c, void *svg);

extern void desenhar_sobreposicoes(void *c, void *svg);

extern void desenhar_elementos(void *this, void *svg);

#endif /* __CONTROLADOR_R__ */