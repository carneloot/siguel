#ifndef __FIGURA_H__
#define __FIGURA_H__

#include <model/modules/ponto2d.h>

/**
 * Tipo abstrato para representar as figuras do programa. Pode ser do tipo
 * Circulo ou do tipo Retangulo
 */
typedef void *Figura;

enum FiguraTracejada { FIG_BORDA_SOLIDA, FIG_BORDA_TRACEJADA };
enum TipoFigura { CIRCULO, RETANGULO };


Figura cria_retangulo(
  double x, double y, double w, double h, char *cor, char *cor_borda);

Figura cria_circulo(double x, double y, double r, char *cor, char *cor_borda);

int sobrepoe_figura(Figura f, Figura f2);

int contem_ponto(Figura f, Ponto2D ponto);

Ponto2D get_centro_massa(Figura f);

Figura get_rect_sobreposicao(Figura f1, Figura f2, char* cor_borda);

double distancia_figuras(Figura f, Figura f2);

void destruir_figura(Figura f);

/**
 * Verifica se other está dentro de this.
 */
int dentro_figura(Figura this, Figura other);

/** Getters */

Ponto2D get_pos(Figura f);

double get_x(Figura f);

double get_y(Figura f);

double get_r(Figura f);

double get_w(Figura f);

double get_h(Figura f);

char *get_cor(Figura f);

char *get_cor_borda(Figura f);

enum TipoFigura get_tipo_figura(Figura f);

int get_id_figura(Figura f);

char *get_svg_figura(Figura this);

/** Setters */

void set_id_figura(Figura f, int id);

void set_opacity_figura(Figura this, double opacity);

void set_dashed_figura(Figura this, int dashed);

void set_stroke_size_figura(Figura this, double size);

#endif /* __FIGURA_H__ */