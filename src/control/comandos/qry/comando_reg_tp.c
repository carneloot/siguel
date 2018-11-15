#include <model/comando.r>
#include <control/controlador.r>

#include <model/comercio.h>
#include <model/endereco.h>
#include <model/modules/logger.h>
#include <float.h>
#include <stdlib.h>

/**
 * Comando:    @tp?
 * Parametros: r1 tp r2
 * Descricao:  Armazena no registrador r1 o
 *             estabelecimento comercial do tipo tp mais
 *             próximo da posição armazenada no
 *             registrador r2
 * 
 */

int search_tp(void * const _comercio, const void *_tp) {
  Comercio comercio = _comercio;
  const char *tp = (const char *) _tp;

  return strcmp(tp, comercio_get_tipo(comercio));
}

int comando_reg_tp(void *_this, void *_controlador) {
  struct Comando *this = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  // Parametros
  int reg_resultado = strtol(this->params[0] + 1, NULL, 10);
  int reg_consulta  = strtol(this->params[2] + 1, NULL, 10);
  char *tipo        = this->params[1];

  Ponto2D posicao = controlador->registradores[reg_consulta];

  // Itera por todos os do tipo especificado encontrando o mais proximo
  // Sei que isso é muito porco, mas os comercios sao guardados numa lista,
  // e mesmo se fossem guardados numa KDTree, os tipo sao diferentes.
  // Entao esse foi o melhor jeito que encontrei de fazer esse comando
  Posic it = Lista_t.get_first(controlador->comercios);
  it = Lista_t.search(controlador->comercios, it, tipo, search_tp);

  double menor_distancia_quadrado = DBL_MAX;
  Ponto2D ponto_mais_proximo = Ponto2D_t.new(0, 0);

  while (it) {
    Comercio comercio = Lista_t.get(controlador->comercios, it);

    Endereco endereco_comercio = comercio_get_endereco(comercio);

    Ponto2D posicao_comercio = endereco_get_coordenada(endereco_comercio, controlador);

    double distancia_atual_quadrado = Ponto2D_t.dist_squared(posicao, posicao_comercio);

    if (distancia_atual_quadrado < menor_distancia_quadrado) {
      distancia_atual_quadrado = menor_distancia_quadrado;
      ponto_mais_proximo = posicao_comercio;
    }

    it = Lista_t.get_next(controlador->comercios, it);
    it = Lista_t.search(controlador->comercios, it, tipo, search_tp);
  }

  controlador->registradores[reg_resultado] = ponto_mais_proximo;

  return 1;
}