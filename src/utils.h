#ifndef __UTILS_H__
#define __UTILS_H__

#define max(a, b) ((a > b) ? (a) : (b))

#define min(a, b) ((a < b) ? (a) : (b))

#define mod(a) ((a < 0) ? (-a) : (a))

#define sqr(a) ((a) * (a))

char *trim(char *);

char *get_nome(char *path);

char *get_diretorio(char *path);

char *get_extensao(char *path);

char *evaluate_dir(char *dir);

char *remover_extensao(const char *path);

char *format_string(const char *fmt, ...);

#endif /* __UTILS_H__ */