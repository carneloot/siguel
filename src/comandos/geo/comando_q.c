#include <comando.r>
#include <controlador.r>

#include <elemento.h>
#include <string.h>
#include <stdlib.h>
#include <utils.h>

int __comando_q(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;

  char *cep   = params[0];
  Ponto2D pos = Ponto2D_t.new( strtod(params[1], NULL), strtod(params[2], NULL));
  Ponto2D size =
    Ponto2D_t.new(strtod(params[3], NULL), strtod(params[4], NULL));

  Elemento elemento = cria_quadra(pos.x, pos.y, cep, size.x, size.y);

  set_cor_elemento(elemento, controlador->cores[QUADRA]);
  set_cor_borda_elemento(elemento, controlador->cores_borda[QUADRA]);

  KDTree_t.insert(controlador->elementos[QUADRA], elemento);

  // Inserir na HashTable a quadra
  HashInfo info;
  info.chave = get_cep_elemento(elemento);
  info.valor = elemento;

  HashTable_t.insert(controlador->tabelas[CEP_X_QUADRA], info);

  Ponto2D new_max = Ponto2D_t.add(size, pos);
  new_max         = Ponto2D_t.add_scalar(new_max, 4);

  controlador->max_geo.x = max(controlador->max_geo.x, new_max.x);
  controlador->max_geo.y = max(controlador->max_geo.y, new_max.y);

  controlador->max_qry.x = max(controlador->max_qry.x, new_max.x);
  controlador->max_qry.y = max(controlador->max_qry.y, new_max.y);

  return 1;
}