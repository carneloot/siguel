#ifndef __CLOSEST_H__
#define __CLOSEST_H__

struct Point {
  float x, y;
};

typedef struct Point *Point;

float closest(Point p[], int n);

#endif /* __CLOSEST_H__ */