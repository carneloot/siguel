#ifndef __CLOSEST_H__
#define __CLOSEST_H__

typedef void *Point;

typedef void *ClosestPair;

ClosestPair closest(void **p, int n);

Point get_point1(ClosestPair this);

Point get_point2(ClosestPair this);

float get_dist(ClosestPair this);

#endif /* __CLOSEST_H__ */