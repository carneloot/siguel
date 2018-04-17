#ifndef _FIGURA_H_
#define _FIGURA_H_

typedef struct Figura* Figura;

Figura cria_retangulo(float x, float y, float w, float h, char* cor, char* cor_borda);

Figura cria_circulo(float x, float y, float r, char* cor, char* cor_borda);

int intercepta_figura(Figura f, Figura f2);

#endif // _FIGURA_H_