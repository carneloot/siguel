#include <comando.r>
#include <controlador.r>

#include <pessoa.h>

int comando_pm_p(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;

  int sexo;
  
  switch (params[3][0]) {
    case 'M': sexo = SEXO_MASCULINO; break;
    case 'F': sexo = SEXO_FEMININO;  break;
  }

  Pessoa pessoa = cria_pessoa(
    params[0], params[1], params[2], sexo, params[4]
  );

  Lista_t.insert(controlador->pessoas, pessoa);

  HashInfo info;

  info.chave = pessoa_get_cpf(pessoa);
  info.valor = pessoa;

  HashTable_t.insert(controlador->tabelas[CPF_X_PESSOA], info);

  return 1;
}