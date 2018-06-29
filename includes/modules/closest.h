#ifndef __CLOSEST_H__
#define __CLOSEST_H__

typedef void *Point;

typedef void *ClosestPair;

ClosestPair closest_pair(void **p, int n);

Point get_point1_pair(ClosestPair this);

Point get_point2_pair(ClosestPair this);

float get_dist_pair(ClosestPair this);

#endif /* __CLOSEST_H__ */