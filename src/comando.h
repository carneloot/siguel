#ifndef _COMANDO_H_
#define _COMANDO_H_

typedef struct Comando* Comando;
typedef enum TipoComando TipoComando;

Comando cria_comando(char* entrada);

int get_numero_parametros(Comando c);

char** get_parametros(Comando c);

TipoComando get_tipo(Comando c);

#endif // _COMANDO_H_