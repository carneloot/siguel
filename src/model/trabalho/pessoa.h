#ifndef __PESSOA_H__
#define __PESSOA_H__

#include <model/trabalho/endereco.h>

typedef void *Pessoa;

enum Sexo { SEXO_MASCULINO, SEXO_FEMININO };

/**
 * Retorna uma pessoa de acordo com as informacoes passadas
 */
Pessoa cria_pessoa(char *cpf, char *nome, char *sobrenome, int sexo, char *nasc);

void pessoa_set_endereco(Pessoa this, char *cep, int face, int num, char *complemento);

void pessoa_remover_endereco(Pessoa this);

char *pessoa_get_cpf(Pessoa this);

char *pessoa_get_cep(Pessoa this);

char *pessoa_get_nome(Pessoa this);

char *pessoa_get_info(Pessoa _this, void *controlador);

Endereco pessoa_get_endereco(Pessoa this);

void pessoa_destruir(Pessoa this);

#endif /* __PESSOA_H__ */