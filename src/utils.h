#ifndef __UTILS_H__
#define __UTILS_H__

float round_digits(float num, int n);

void trim(char **);

int num_digits(float);

float dist_squared(float x1, float y1, float x2, float y2);

float dist(float x1, float y1, float x2, float y2);

char *get_nome(char *path);

char *get_diretorio(char *path);

char *get_extensao(char *path);

#endif  // __UTILS_H__