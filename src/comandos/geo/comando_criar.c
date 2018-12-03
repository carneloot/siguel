#include <comando.r>
#include <controlador.r>

#include <string.h>
#include <stdlib.h>

#include <elemento.h>
#include <utils.h>
#include <figura.h>
#include <utils.h>

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
  Ponto2D_t pos = p2d_new(strtod(params[4], NULL), strtod(params[5], NULL));

  Figura figura = cria_circulo(pos.x, pos.y, r, cor, cor_borda);
  set_id_figura(figura, id);

  lt_insert((Lista_t) controlador->figuras, figura);
  
  Ponto2D_t new_max = p2d_add_scalar(pos, r + 4);

  controlador->max_geo = p2d_maximo(controlador->max_geo, new_max);
  
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

  Ponto2D_t size =
    p2d_new(strtod(params[3], NULL), strtod(params[4], NULL));
  Ponto2D_t pos = p2d_new(strtod(params[5], NULL), strtod(params[6], NULL));

  Figura figura = cria_retangulo(pos.x, pos.y, size.x, size.y, cor, cor_borda);
  set_id_figura(figura, id);

  lt_insert((Lista_t) controlador->figuras, figura);

  Ponto2D_t new_max = p2d_add(pos, size);
  new_max = p2d_add_scalar(new_max, 4);

  controlador->max_geo = p2d_maximo(controlador->max_geo, new_max);

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
  Ponto2D_t pos = p2d_new( strtod(params[1], NULL), strtod(params[2], NULL));
  Ponto2D_t size =
    p2d_new(strtod(params[3], NULL), strtod(params[4], NULL));

  Elemento elemento = cria_quadra(pos.x, pos.y, cep, size.x, size.y);

  set_cor_elemento(elemento, controlador->cores[QUADRA]);
  set_cor_borda_elemento(elemento, controlador->cores_borda[QUADRA]);

  kdt_insert(controlador->elementos[QUADRA], elemento);

  // Inserir na HashTable a quadra
  HashTable_t.insert(controlador->tabelas[CEP_X_QUADRA], get_cep_elemento(elemento), elemento);

  Ponto2D_t new_max = p2d_add(size, pos);
  new_max         = p2d_add_scalar(new_max, 4);

  controlador->max_geo = p2d_maximo(controlador->max_geo, new_max);
  controlador->max_qry = p2d_maximo(controlador->max_qry, new_max);

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
  Ponto2D_t pos = p2d_new(strtod(params[1], NULL), strtod(params[2], NULL));

  Elemento elemento = cria_hidrante(pos.x, pos.y, cep);

  set_cor_elemento(elemento, controlador->cores[HIDRANTE]);
  set_cor_borda_elemento(elemento, controlador->cores_borda[HIDRANTE]);

  kdt_insert(controlador->elementos[HIDRANTE], elemento);

  HashTable_t.insert(controlador->tabelas[ID_X_HIDRANTE], get_id_elemento(elemento), elemento);

  Ponto2D_t new_max = p2d_add_scalar(pos, RAIO_EQUIPAMENTOS + 4);

  controlador->max_geo = p2d_maximo(controlador->max_geo, new_max);
  controlador->max_qry = p2d_maximo(controlador->max_qry, new_max);

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
  Ponto2D_t pos = p2d_new(strtod(params[1], NULL), strtod(params[2], NULL));

  Elemento elemento = cria_semaforo(pos.x, pos.y, cep);

  set_cor_elemento(elemento, controlador->cores[SEMAFORO]);
  set_cor_borda_elemento(elemento, controlador->cores_borda[SEMAFORO]);

  kdt_insert(controlador->elementos[SEMAFORO], elemento);

  HashTable_t.insert(controlador->tabelas[ID_X_SEMAFORO], get_id_elemento(elemento), elemento);

  Ponto2D_t new_max = p2d_add_scalar(pos, RAIO_EQUIPAMENTOS + 4);

  controlador->max_geo = p2d_maximo(controlador->max_geo, new_max);
  controlador->max_qry = p2d_maximo(controlador->max_qry, new_max);

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
  Ponto2D_t pos = p2d_new(strtod(params[1], NULL), strtod(params[2], NULL));

  Elemento elemento = cria_radio_base(pos.x, pos.y, cep);

  set_cor_elemento(elemento, controlador->cores[RADIO_BASE]);
  set_cor_borda_elemento(elemento, controlador->cores_borda[RADIO_BASE]);

  kdt_insert(controlador->elementos[RADIO_BASE], elemento);

  HashTable_t.insert(controlador->tabelas[ID_X_RADIO], get_id_elemento(elemento), elemento);

  Ponto2D_t new_max = p2d_add_scalar(pos, RAIO_EQUIPAMENTOS + 4);

  controlador->max_geo = p2d_maximo(controlador->max_geo, new_max);

  controlador->max_qry = p2d_maximo(controlador->max_qry, new_max);

  return 1;
}