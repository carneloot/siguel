#ifndef __FIGURA_H__
#define __FIGURA_H__

/**
 * Tipo abstrato para representar as figuras do programa. Pode ser do tipo
 * Circulo ou do tipo Retangulo
 */
typedef void *Figura;

enum TipoFigura { CIRCULO, RETANGULO };

/**
 * 'cor' e 'cor_borda' devem ser cores válidas de SVG.
 * Retorna uma Figura do tipo Retângulo com as especificações dadas.
 */
Figura cria_retangulo(
  float x, float y, float w, float h, char *cor, char *cor_borda);

Figura cria_circulo(float x, float y, float r, char *cor, char *cor_borda);

int sobrepoe_figura(Figura f, Figura f2);

int contem_ponto(Figura f, double x, double y);

void get_centro_massa(Figura f, float *x, float *y);

Figura get_rect_sobreposicao(Figura f1, Figura f2);

float distancia_figuras(Figura f, Figura f2);

void destruir_figura(Figura f);

/**
 * Verifica se other está dentro de this.
 */
int dentro_figura(Figura this, Figura other);

/** Getters */

float get_x(Figura f);

float get_y(Figura f);

float get_r(Figura f);

float get_w(Figura f);

float get_h(Figura f);

char *get_cor(Figura f);

char *get_cor_borda(Figura f);

enum TipoFigura get_tipo_figura(Figura f);

int get_id_figura(Figura f);

/** Setters */

void set_id_figura(Figura f, int id);

#endif /* __FIGURA_H__ */