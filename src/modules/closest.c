#include "modules/closest.h"

#include <math.h>
#include <stdlib.h>
#include "modules/sorts.h"
#include "utils.h"

int compareX(const void *a, const void *b) {
  Point p1 = (Point) a;
  Point p2 = (Point) b;
  return (p1->x - p2->x);
}

int compareY(const void *a, const void *b) {
  Point p1 = (Point) a;
  Point p2 = (Point) b;
  return (p1->y - p2->y);
}

float distance(Point p1, Point p2) {
  if (p1 == p2)
    return INFINITY;
  float dx, dy;
  dx = p1->x - p2->x;
  dy = p1->y - p2->y;
  return sqrt(dx * dx + dy * dy);
}

float bruteForce(Point p[], int n) {
  float min = INFINITY;
  for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++)
      if (distance(p[i], p[j]) < min)
        min = distance(p[i], p[j]);
  return min;
}

float stripClosest(Point strip[], int size, float d) {
  float minimo = d, dist;

  heap_sort((void **) strip, size, compareY);

  for (int i = 0; i < size; i++) {
    for (int j = i + 1; j < size && (strip[j]->y - strip[i]->y) < minimo; j++) {
      dist = distance(strip[i], strip[j]);
      if (dist < minimo)
        minimo = dist;
    }
  }

  return minimo;
}

float closestUtil(Point p[], int n) {
  if (n <= 3)
    return bruteForce(p, n);

  int mid        = n / 2;
  Point midPoint = p[mid];

  float dl = closestUtil(p, mid);
  float dr = closestUtil(p + mid, n - mid);

  float d = min(dl, dr);

  Point strip[n];
  int j = 0;
  for (int i = 0; i < n; i++)
    if (mod(p[i]->x - midPoint->x) < d)
      strip[j++] = p[i];

  return min(d, stripClosest(strip, j, d));
}

float closest(Point p[], int n) {
  heap_sort((void **) p, n, compareX);

  return closestUtil(p, n);
}