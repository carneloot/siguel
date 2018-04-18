#ifndef __FIGURA_H__
#define __FIGURA_H__

typedef void *Figura;

Figura cria_retangulo(float x, float y, float w, float h, char *cor,
                      char *cor_borda);

Figura cria_circulo(float x, float y, float r, char *cor, char *cor_borda);

int intercepta_figura(Figura f, Figura f2);

#endif  // __FIGURA_H__