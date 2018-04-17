#ifndef _ARQUIVO_H_
#define _ARQUIVO_H_

typedef struct Arquivo* Arquivo;

typedef enum TipoArquivo TipoArquivo;

Arquivo abrir_arquivo(char* path, TipoArquivo modo);

char* ler_proxima_linha(Arquivo a);

char* escrever_linha(Arquivo a, char* entrada);

int numero_linha_atual(Arquivo a);

void fechar_arquivo(Arquivo a);

#endif // _ARQUIVO_H_