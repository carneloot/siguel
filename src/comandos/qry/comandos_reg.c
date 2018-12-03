#include <comando.r>
#include <controlador.r>

#include <comercio.h>
#include <endereco.h>
#include <elemento.h>
#include <pessoa.h>

#include <modules/logger.h>

#include <float.h>
#include <stdlib.h>
#include <string.h>

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

/**
 * Comando:    @m?
 * Parametros: r cpf
 * Descricao:  Armazena no registrador r a posição geográfica da residência do morador de cpf cpf
 */
int comando_reg_m(void *_this, void *_controlador) {
  struct Comando *this = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  int numero_registrador = strtol(this->params[0] + 1, NULL, 10);
  char *cpf = this->params[1];

  Pessoa pessoa = HashTable_t.get(controlador->tabelas[CPF_X_PESSOA], cpf);

  if (pessoa == NULL) {
    LOG_ERRO("Nao foi possivel encontrar a pessoa de CPF \"%s\".", cpf);
    return 1;
  }

  Endereco endereco_pessoa = pessoa_get_endereco(pessoa);

  Ponto2D coordenadas = endereco_get_coordenada(endereco_pessoa, controlador);

  controlador->registradores[numero_registrador] = coordenadas;

  return 1;
}

/**
 * Comando:    @g?
 * Parametros: r id
 * Descricao:  Armazena no registrador r a posição geográfica
 *             do equipamento urbano cujo identificador é id
 */
int comando_reg_g(void *_this, void *_controlador) {
  struct Comando *this = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  int numero_registrador = strtol(this->params[0] + 1, NULL, 10);
  char *id_equipamento = this->params[1];

  Elemento encontrado = NULL;

  for (int i = ID_X_RADIO; i <= ID_X_SEMAFORO; i++) {
    if (HashTable_t.exists(controlador->tabelas[i], id_equipamento)) {
      encontrado = HashTable_t.get(controlador->tabelas[i], id_equipamento);
      break;
    }
  }

  if (encontrado == NULL) {
    LOG_ERRO("Equipamento com id \"%d\" nao encontrado.", id_equipamento);
    return 0;
  }

  controlador->registradores[numero_registrador] = get_pos(encontrado);

  return 1;
}

/**
 * Comando: @xy
 * Params: r x y
 * Armazena a posição (x,y) no registrador r
 */
int comando_reg_xy(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;

  int numero_registrador    = strtol(params[0] + 1, NULL, 10);
  double x                  = strtod(params[1], NULL);
  double y                  = strtod(params[2], NULL);

  Ponto2D ponto = Ponto2D_t.new(x, y);
  controlador->registradores[numero_registrador] = ponto;

  return 1;
}

/** 
 * Comando: @e
 * Params:  r cep face num
 * Armazena em r a posição geográfica do endereço
 *   Cep face num
 */

/**
 * Criar endereço
 * Pegar coordenadas do endereço
 * Guardar no registrador
 */
int comando_reg_e(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char **params = this->params;

  int  numero_registrador = strtol( params[0] + 1, NULL, 10 );

  char *cep          = params[1];
  char *char_face    = params[2];
  int  numero        = strtol( params[3], NULL, 10 );
  int  face;

  switch( char_face[0] ){
    case 'N': face = FACE_NORTE; break;
    case 'S': face = FACE_SUL;   break;
    case 'L': face = FACE_LESTE; break;
    case 'O': face = FACE_OESTE; break;
  }

  Endereco endereco;
  endereco = cria_endereco( cep, face, numero);
  Ponto2D ponto = endereco_get_coordenada(endereco, controlador);

  controlador->registradores[numero_registrador].x = ponto.x;
  controlador->registradores[numero_registrador].y = ponto.y;

  endereco_destruir(endereco);

  return 1;
}