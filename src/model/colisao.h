#ifndef __COLISAO_H__
#define __COLISAO_H__

#include <model/modules/lista.h>
#include <model/figura.h>

typedef void* Colisao;

Colisao colisao_create( Figura figura, Lista arestas );

void colisao_destroy( Colisao this );

Lista colisao_get_arestas( Colisao this );

Figura colisao_get_figura( Colisao this );




#endif /* __COLISAO_H__ */