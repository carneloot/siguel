#ifndef __DESENHAVEL_H__
#define __DESENHAVEL_H__

struct Desenhavel {
  void *valor;
  char *(*to_svg)(void *valor);
  void (*destruir)(void *valor);
};

typedef struct Desenhavel *Desenhavel;

Desenhavel cria_desenhavel(void *valor, char *(*to_svg)(void *valor), void (*destruir)(void *valor));

void desenhavel_destruir(void *this);

#endif /* __DESENHAVEL_H__ */