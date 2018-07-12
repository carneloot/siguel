#include "modules/closest.h"

#include <math.h>
#include <stdlib.h>
#include "modules/sorts.h"
#include "utils.h"

struct Point {
  float x, y;
};

static int compareX(const void *a, const void *b) {
  struct Point *p1 = (struct Point *) a;
  struct Point *p2 = (struct Point *) b;
  return (p1->x - p2->x);
}

static int compareY(const void *a, const void *b) {
  struct Point *p1 = (struct Point *) a;
  struct Point *p2 = (struct Point *) b;
  return (p1->y - p2->y);
}

static float distance(struct Point *p1, struct Point *p2) {
  if (p1 == p2)
    return INFINITY;
  float dx, dy;
  dx = p1->x - p2->x;
  dy = p1->y - p2->y;
  return sqrt(dx * dx + dy * dy);
}

static ClosestPair bruteForce(struct Point **p, int n) {
  ClosestPair min;

  min.dist   = INFINITY;
  min.point1 = NULL;
  min.point2 = NULL;

  for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++)
      if (distance(p[i], p[j]) < min.dist) {
        min.dist   = distance(p[i], p[j]);
        min.point1 = p[i];
        min.point2 = p[j];
      }
  return min;
}

static ClosestPair stripClosest(struct Point **strip, int size, ClosestPair d) {

  ClosestPair min = d;
  float dist;
  int i, j;

  heap_sort((void **) strip, size, compareY);

  for (i = 0; i < size; i++) {
    for (j = i + 1; j < size && (strip[j]->y - strip[i]->y) < min.dist; j++) {
      dist = distance(strip[i], strip[j]);
      if (dist < min.dist) {
        min.dist   = dist;
        min.point1 = strip[i];
        min.point2 = strip[j];
      }
    }
  }

  return min;
}

static ClosestPair closestUtil(void **_p, int n) {
  struct Point **p = (struct Point **) _p;

  if (n <= 3)
    return bruteForce(p, n);

  int mid = n / 2;

  struct Point *midPoint = p[mid];

  ClosestPair dl = closestUtil(_p, mid);
  ClosestPair dr = closestUtil(_p + mid, n - mid);

  ClosestPair d = (dl.dist < dr.dist) ? dl : dr;

  struct Point *strip[n];
  int j = 0;
  for (int i = 0; i < n; i++)
    if (mod(p[i]->x - midPoint->x) < d.dist)
      strip[j++] = p[i];

  struct ClosestPair new_d = stripClosest(strip, j, d);

  return (d.dist < new_d.dist) ? d : new_d;
}

ClosestPair closest_pair(void **p, int n) {
  heap_sort((void **) p, n, compareX);

  return closestUtil(p, n);
}