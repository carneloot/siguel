#ifndef __ARQUIVO_H__
#define __ARQUIVO_H__

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
 * 'a' deve ser do modo escrita ou alteracao
 * Escreve em 'a' a string 'entrada'
 */
void escrever_linha(Arquivo a, char *entrada);

/**
 * Retorna a linha atual do arquivo 'a'
 */
int numero_linha_atual(Arquivo a);

/**
 * Fecha o arquivo 'a'. Ele deve ter sido aberto anteriormente.
 */
void fechar_arquivo(Arquivo a);

#endif  // __ARQUIVO_H__