#include "modules/closest.h"

#include <math.h>
#include <stdlib.h>
#include "modules/sorts.h"
#include "utils.h"

struct Point {
  float x, y;
};

struct ClosestPair {
  float dist;
  void *point1;
  void *point2;
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

static float distance(Point _p1, Point _p2) {
  struct Point *p1 = (struct Point *) _p1;
  struct Point *p2 = (struct Point *) _p2;

  if (p1 == p2)
    return INFINITY;
  float dx, dy;
  dx = p1->x - p2->x;
  dy = p1->y - p2->y;
  return sqrt(dx * dx + dy * dy);
}

static ClosestPair bruteForce(Point p[], int n) {
  struct ClosestPair *min = calloc(1, sizeof(struct ClosestPair));

  min->dist   = INFINITY;
  min->point1 = NULL;
  min->point2 = NULL;

  for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++)
      if (distance(p[i], p[j]) < min->dist) {
        min->dist   = distance(p[i], p[j]);
        min->point1 = p[i];
        min->point2 = p[j];
      }
  return (ClosestPair) min;
}

static ClosestPair stripClosest(void **_strip, int size, ClosestPair d) {
  struct Point **strip       = (struct Point **) _strip;
  struct ClosestPair *minimo = d;
  float dist;

  heap_sort((void **) strip, size, compareY);

  for (int i = 0; i < size; i++) {
    for (int j = i + 1; j < size && (strip[j]->y - strip[i]->y) < minimo->dist;
         j++) {
      dist = distance(strip[i], strip[j]);
      if (dist < minimo->dist) {
        minimo->dist   = dist;
        minimo->point1 = strip[i];
        minimo->point2 = strip[j];
      }
    }
  }

  return minimo;
}

static ClosestPair closestUtil(void **_p, int n) {
  struct Point **p = (struct Point **) _p;

  if (n <= 3)
    return bruteForce((Point *) p, n);

  int mid = n / 2;

  struct Point *midPoint = p[mid];

  struct ClosestPair *dl = closestUtil(_p, mid);
  struct ClosestPair *dr = closestUtil(_p + mid, n - mid);

  struct ClosestPair *d;
  if (dl->dist < dr->dist) {
    d = dl;
    free(dr);
  } else {
    d = dr;
    free(dl);
  }

  void *strip[n];
  int j = 0;
  for (int i = 0; i < n; i++)
    if (mod(p[i]->x - midPoint->x) < d->dist)
      strip[j++] = p[i];

  struct ClosestPair *new_d = stripClosest(strip, j, d);

  return (d->dist < new_d->dist) ? d : new_d;
}

ClosestPair closest_pair(void **p, int n) {
  heap_sort((void **) p, n, compareX);

  return closestUtil(p, n);
}

Point get_point1_pair(ClosestPair this) {
  return (Point) ((struct ClosestPair *) this)->point1;
}

Point get_point2_pair(ClosestPair this) {
  return (Point) ((struct ClosestPair *) this)->point2;
}

float get_dist_pair(ClosestPair this) {
  return ((struct ClosestPair *) this)->dist;
}