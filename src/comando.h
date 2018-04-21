#ifndef __COMANDO_H__
#define __COMANDO_H__

enum TipoComando {
  NONE,
  MUDAR_NUM_FIGURAS,
  DESENHA_CIRCULO,
  DESENHA_RETANGULO,
  CHECA_SOBREPOSICAO,
  CHECA_PONTO,
  DISTANCIA_FIGURAS,
  CRIAR_SVG,
  FIM_DO_ARQUIVO
};

typedef void *Comando;

Comando cria_comando(char *entrada);

int get_numero_parametros(Comando c);

char **get_parametros(Comando c);

enum TipoComando get_tipo(Comando c);

void destruir_comando(Comando c);

#endif  // __COMANDO_H__