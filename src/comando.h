#ifndef __COMANDO_H__
#define __COMANDO_H__

typedef void* Comando;
typedef int TipoComando;

Comando cria_comando(char* entrada);

int get_numero_parametros(Comando c);

char** get_parametros(Comando c);

TipoComando get_tipo(Comando c);

#endif  // __COMANDO_H__