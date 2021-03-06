#include <comando.r>
#include <controlador.r>

#include <model/trabalho/elemento.h>

/**
 * Comando: cc
 * Params:  ( cep | id ) cstrk cfill
 * Muda as cores do contorno e do
 * preenchimento da quadra identificada por
 * cep ou do equipamento urbano
 * identificado por id.
 * Saída: arquivo.svg: quadra ou equipamento
 * pintados com as novas cores.
 */
int comando_qry_cc(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;

  char *info = params[0];

  char *cor_borda = params[1];
  char *cor       = params[2];

  for (int i = CEP_X_QUADRA; i < ID_X_SEMAFORO; i++) {
    HashTable_t tabela = controlador->tabelas[i];

    // Se nao achar nessa tabela, vai para a proxima
    if (!ht_exists(tabela, info))
      continue;

    Elemento elemento = ht_get(tabela, info);

    set_cor_elemento(elemento, cor);
    set_cor_borda_elemento(elemento, cor_borda);

    break;
  }

  return 1;
}