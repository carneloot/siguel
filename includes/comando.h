#ifndef __COMANDO_H__
#define __COMANDO_H__

enum TipoComando {
  NONE,
  GEO_MUDAR_NUM_FIGURAS,
  GEO_DESENHA_CIRCULO,
  GEO_DESENHA_RETANGULO,
  GEO_CHECA_SOBREPOSICAO,
  GEO_CHECA_PONTO,
  GEO_DISTANCIA_FIGURAS,
  GEO_CRIAR_SVG,
  GEO_FIM_DO_ARQUIVO,
  GEO_INSERE_QUADRA,
  GEO_INSERE_HIDRANTE,
  GEO_INSERE_SEMAFORO,
  GEO_INSERE_RADIO_BASE,
  GEO_COR_QUADRA,
  GEO_COR_HIDRANTE,
  GEO_COR_RADIO_BASE,
  GEO_COR_SEMAFORO,

  // Comandos .qry
  QRY_BUSCA_RECT,
  QRY_BUSCA_CIRC,
  QRY_DELETE_QUADRA_RECT,
  QRY_DELETE_ALL_RECT,
  QRY_DELETE_QUADRA_CIRC,
  QRY_DELETE_ALL_CIRC,
  QRY_MUDA_COR_QUADRA,
  QRY_PRINT_EQUIPAMENTO,
  QRY_CHECA_RADIO_BASE_PROXIMA
};

typedef void *Comando;

Comando cria_comando(char *entrada);

int get_numero_parametros(Comando c);

char **get_parametros(Comando c);

enum TipoComando get_tipo(Comando c);

void destruir_comando(Comando c);

#endif /* __COMANDO_H__ */