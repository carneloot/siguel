#ifndef __UTILS_H__
#define __UTILS_H__

#define max(a, b) ((a > b) ? (a) : (b))

#define min(a, b) ((a < b) ? (a) : (b))

#define mod(a) ((a < 0) ? (-a) : (a))

char *trim(char *);

float dist_squared(float x1, float y1, float x2, float y2);

float dist(float x1, float y1, float x2, float y2);

char *get_nome(char *path);

char *get_diretorio(char *path);

char *get_extensao(char *path);

char *evaluate_dir(char *dir);

#endif /* __UTILS_H__ */