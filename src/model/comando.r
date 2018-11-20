#ifndef __COMANDO_R__
#define __COMANDO_R__

struct Comando {
  int num_param;
  char **params;
  char *string;
  int (*executar)(void *this, void *controlador);
};

#endif /* __COMANDO_R__ */