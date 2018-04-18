#ifndef __ARQUIVO_H__
#define __ARQUIVO_H__

typedef void *Arquivo;
typedef int TipoArquivo;

Arquivo abrir_arquivo(char *path, TipoArquivo modo);

char *ler_proxima_linha(Arquivo a);

char *escrever_linha(Arquivo a, char *entrada);

int numero_linha_atual(Arquivo a);

void fechar_arquivo(Arquivo a);

#endif  // __ARQUIVO_H__