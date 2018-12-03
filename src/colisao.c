#include "colisao.h"

#include <mapa_viario/aresta.h>

#include <stdlib.h>


struct Colisao{
    Figura figura;
    Lista arestas;
};

Colisao colisao_create(Figura figura, Lista arestas){
    struct Colisao* this = malloc(sizeof(struct Colisao));
    this->arestas = arestas;
    this->figura = figura;
    return this;
}

void colisao_destroy(Colisao _this){
    struct Colisao* this = _this;
    Lista_t.destruir( this->arestas, NULL );
    destruir_figura( this->figura );
    free(this);
}

Lista colisao_get_arestas( Colisao _this ){
    struct Colisao* this = _this;
    return this->arestas;
}

Figura colisao_get_figura( Colisao _this ){
    struct Colisao* this = _this;
    return this->figura;
}