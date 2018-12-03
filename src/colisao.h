#ifndef __COLISAO_H__
#define __COLISAO_H__

#include <modules/lista.h>
#include <figura.h>

typedef void* Colisao;

Colisao colisao_create( Figura figura, Lista_t arestas );

void colisao_destroy( Colisao this );

Lista_t colisao_get_arestas( Colisao this );

Figura colisao_get_figura( Colisao this );




#endif /* __COLISAO_H__ */