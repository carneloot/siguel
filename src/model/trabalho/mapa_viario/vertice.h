#ifndef __VERTICE_H__
#define __VERTICE_H__

#include <model/estrutural/ponto2d.h>

struct VerticeInfo {
  Ponto2D_t pos;
  char *id;
};

typedef struct VerticeInfo *VerticeInfo;

VerticeInfo create_vertice_info(Ponto2D_t pos, char *id);

void destroy_vertice_info(void *this);

#endif /* __VERTICE_H__ */