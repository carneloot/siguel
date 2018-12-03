#include "vertice.h"

#include <stdlib.h>
#include <string.h>

VerticeInfo create_vertice_info(Ponto2D_t pos, char *id) {
  VerticeInfo this = calloc(1, sizeof(*this));

  this->pos = pos;

  this->id = calloc(strlen(id) + 1, sizeof(char));

  strcpy(this->id, id);

  return this;
}

void destroy_vertice_info(void *_this) {
  VerticeInfo this = _this;
  free(this->id);

  free(this);
}