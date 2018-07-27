#ifndef __CLOSEST_H__
#define __CLOSEST_H__

struct ClosestPair {
  double dist;
  void *point1;
  void *point2;
};

typedef struct ClosestPair ClosestPair;

ClosestPair closest_pair(void **p, int n);

#endif /* __CLOSEST_H__ */