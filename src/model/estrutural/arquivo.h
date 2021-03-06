#ifndef __ARQUIVO_H__
#define __ARQUIVO_H__

#include <stdarg.h>

/**
 * Serve para guardar e como interface de um arquivo aberto, podendo ser de
 * leitura ou escrita.
 */
typedef void *Arquivo;

enum TipoArquivo { LEITURA, ESCRITA, ALTERACAO };

/**
 * 'path' deve ser um caminho valido.
 * Retorna o arquivo aberto em path, válido para a operação escolhida.
 */
Arquivo abrir_arquivo(char *path, enum TipoArquivo modo);

/**
 * 'a' deve ser do modo leitura
 * retorna a proxima linha do arquivo a;
 */
char *ler_proxima_linha(Arquivo a);

/**
 * 'this' deve der de modo leitura.
 * Retorna todo o arquivo em uma uma unica string
 */
char *ler_arquivo_inteiro(Arquivo this);

/**
 * 'a' deve ser do modo escrita ou alteracao
 * Escreve em 'a' a string 'entrada'
 */
void escrever_linha(Arquivo a, const char *entrada, ...);

/**
 * Escreve uma linha no arquivo, porem uma lista de argumentos eh passada
 */
void escrever_linha_args(Arquivo a, const char *entrada, va_list args);

/**
 * Retorna a linha atual do arquivo 'a'
 */
int numero_linha_atual(Arquivo a);

/**
 * Fecha o arquivo 'a'. Ele deve ter sido aberto anteriormente.
 */
void fechar_arquivo(Arquivo a);

/**
 * Retorna o caminho do arquivo.
 */
char *get_path(Arquivo a);

#endif /* __ARQUIVO_H__ */