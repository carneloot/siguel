#ifndef __VERTICE_H__
#define __VERTICE_H__

#include <model/modules/ponto2d.h>

struct VerticeInfo {
  Ponto2D pos;
  char *id;
};

typedef struct VerticeInfo *VerticeInfo;

VerticeInfo create_vertice_info(Ponto2D pos, char *id);

void destroy_vertice_info(VerticeInfo this);

#endif /* __VERTICE_H__ */