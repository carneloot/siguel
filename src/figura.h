#ifndef __FIGURA_H__
#define __FIGURA_H__

/**
 * Tipo abstrato para representar as figuras do programa. Pode ser do tipo
 * Circulo ou do tipo Retangulo
 */
typedef void *Figura;

/**
 * 'cor' e 'cor_borda' devem ser cores válidas de SVG.
 * Retorna uma Figura do tipo Retângulo com as especificações dadas.
 */
Figura cria_retangulo(float x, float y, float w, float h, char *cor,
                      char *cor_borda);


Figura cria_circulo(float x, float y, float r, char *cor, char *cor_borda);

int intercepta_figura(Figura f, Figura f2);

int contem_ponto(Figura f, double x, double y);

void destruir_figura(Figura f);

#endif  // __FIGURA_H__