#include <model/comando.r>
#include <control/controlador.r>

#include <string.h>
#include <stdlib.h>

#include <model/elemento.h>
#include <model/utils.h>
#include <model/figura.h>
#include <model/utils.h>

/**
 * Comando: c
 * Params:  i cor1 cor2 r x y
 * desenhar círculo. cor1 é a cor da borda e
 * cor2 é a cor do preenchimento
 */
int comando_geo_c(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;

  int id          = strtol(params[0], NULL, 10);
  char *cor_borda = params[1];
  char *cor       = params[2];
  double r        = strtod(params[3], NULL);
  Ponto2D pos = Ponto2D_t.new(strtod(params[4], NULL), strtod(params[5], NULL));

  Figura figura = cria_circulo(pos.x, pos.y, r, cor, cor_borda);
  set_id_figura(figura, id);

  Lista_t.insert((Lista) controlador->figuras, figura);
  
  Ponto2D new_max = Ponto2D_t.add_scalar(pos, r + 4);

  controlador->max_geo = Ponto2D_t.maximo(controlador->max_geo, new_max);
  
  return 1;
}

/**
 * Comando: r
 * Params:  i cor1 cor2 w h x y
 * desenhar retângulo: w é a largura do
 * retângulo e h, a altura. cor1 é a cor da
 * borda e cor2, a do preenchimento
 */
int comando_geo_r(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;

  int id          = strtol(params[0], NULL, 10);
  char *cor_borda = params[1];
  char *cor       = params[2];

  Ponto2D size =
    Ponto2D_t.new(strtod(params[3], NULL), strtod(params[4], NULL));
  Ponto2D pos = Ponto2D_t.new(strtod(params[5], NULL), strtod(params[6], NULL));

  Figura figura = cria_retangulo(pos.x, pos.y, size.x, size.y, cor, cor_borda);
  set_id_figura(figura, id);

  Lista_t.insert((Lista) controlador->figuras, figura);

  Ponto2D new_max = Ponto2D_t.add(pos, size);
  new_max = Ponto2D_t.add_scalar(new_max, 4);

  controlador->max_geo = Ponto2D_t.maximo(controlador->max_geo, new_max);

  return 1;
}

/**
 * Comando: q
 * Params:  cep x y largura altura
 * Insere uma quadra (retangulo e cep)
 */
int comando_geo_q(void *_this, void *_controlador) {
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

  controlador->max_geo = Ponto2D_t.maximo(controlador->max_geo, new_max);
  controlador->max_qry = Ponto2D_t.maximo(controlador->max_qry, new_max);

  return 1;
}

/**
 * Comando: h
 * Params:  id x y
 * Insere um hidrante
 */
int comando_geo_h(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;

  char *cep   = params[0];
  Ponto2D pos = Ponto2D_t.new(strtod(params[1], NULL), strtod(params[2], NULL));

  Elemento elemento = cria_hidrante(pos.x, pos.y, cep);

  set_cor_elemento(elemento, controlador->cores[HIDRANTE]);
  set_cor_borda_elemento(elemento, controlador->cores_borda[HIDRANTE]);

  KDTree_t.insert(controlador->elementos[HIDRANTE], elemento);

  HashInfo info = {
    .chave = get_id_elemento(elemento),
    .valor = elemento
  };

  HashTable_t.insert(controlador->tabelas[ID_X_HIDRANTE], info);

  Ponto2D new_max = Ponto2D_t.add_scalar(pos, RAIO_EQUIPAMENTOS + 4);

  controlador->max_geo = Ponto2D_t.maximo(controlador->max_geo, new_max);
  controlador->max_qry = Ponto2D_t.maximo(controlador->max_qry, new_max);

  return 1;
}

/**
 * Comando: s
 * Params:  id x y
 * Insere um semaforo
 */
int comando_geo_s(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;

  char *cep   = params[0];
  Ponto2D pos = Ponto2D_t.new(strtod(params[1], NULL), strtod(params[2], NULL));

  Elemento elemento = cria_semaforo(pos.x, pos.y, cep);

  set_cor_elemento(elemento, controlador->cores[SEMAFORO]);
  set_cor_borda_elemento(elemento, controlador->cores_borda[SEMAFORO]);

  KDTree_t.insert(controlador->elementos[SEMAFORO], elemento);

  HashInfo info = {
    .chave = get_id_elemento(elemento),
    .valor = elemento
  };

  HashTable_t.insert(controlador->tabelas[ID_X_SEMAFORO], info);

  Ponto2D new_max = Ponto2D_t.add_scalar(pos, RAIO_EQUIPAMENTOS + 4);

  controlador->max_geo = Ponto2D_t.maximo(controlador->max_geo, new_max);
  controlador->max_qry = Ponto2D_t.maximo(controlador->max_qry, new_max);

  return 1;
}

/**
 * Comando: t
 * Params:  id x y
 * Insere uma torre de celular
 */
int comando_geo_t(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;

  char *cep   = params[0];
  Ponto2D pos = Ponto2D_t.new(strtod(params[1], NULL), strtod(params[2], NULL));

  Elemento elemento = cria_radio_base(pos.x, pos.y, cep);

  set_cor_elemento(elemento, controlador->cores[RADIO_BASE]);
  set_cor_borda_elemento(elemento, controlador->cores_borda[RADIO_BASE]);

  KDTree_t.insert(controlador->elementos[RADIO_BASE], elemento);

  HashInfo info = {
    .chave = get_id_elemento(elemento),
    .valor = elemento
  };
  HashTable_t.insert(controlador->tabelas[ID_X_RADIO], info);

  Ponto2D new_max = Ponto2D_t.add_scalar(pos, RAIO_EQUIPAMENTOS + 4);

  controlador->max_geo = Ponto2D_t.maximo(controlador->max_geo, new_max);

  controlador->max_qry = Ponto2D_t.maximo(controlador->max_qry, new_max);

  return 1;
}