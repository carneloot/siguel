#include <comando.r>
#include <controlador.r>

#include <string.h>
#include <stdlib.h>

#include <comercio.h>
#include <figura.h>
#include <utils.h>

static void *__controlador = NULL;

static int contemCep(void *item, void *other) {
  Comercio comercio = item;
  char *cep   = other;

  char *cep_comercio = comercio_get_endereco(comercio)->cep;

  if (!cep_comercio)
    return 1;

  return strcmp(cep, cep_comercio);
}

static int compararDentro(void * comercio, void *outro) {
  Ponto2D_t *pontos = outro;
  Ponto2D_t pos  = pontos[0];
  Ponto2D_t size = pontos[1];

  Ponto2D_t pos_comercio = endereco_get_coordenada(comercio_get_endereco(comercio), __controlador);

  Figura ret = cria_retangulo(pos.x, pos.y, size.x, size.y, "", "");

  int result = contem_ponto(ret, pos_comercio);

  destruir_figura(ret);
  
  return !result;
}

static int compararCEP(void * comercio, void *outro) {
  return strcmp(comercio_get_endereco(comercio)->cep, outro);
}

static int compararTIPO(void * comercio, void *outro) {
  return strcmp(comercio_get_tipo(comercio), outro);
}

char **__get_tipos(Lista_t comercios) {
  size_t tam = lt_length(comercios);

  if (tam == 0) return NULL;

  char **tipos = calloc(tam + 1, sizeof(*tipos));

  // Peguei todos os tipos
  Posic_t it = lt_get_first(comercios);
  while (it) {
    Comercio comercio = lt_get(comercios, it);
    char *tipo_atual = comercio_get_tipo(comercio);

    char **tipos_it = tipos;

    // Aumenta o tipos_it até ficar vazio ou achar um igual
    while (*tipos_it != NULL && strcmp(tipo_atual, *tipos_it++));
    *tipos_it = tipo_atual;

    it = lt_get_next(comercios, it);
  }

  return tipos;
}

Lista_t __get_comercios_area(Lista_t comercios, Ponto2D_t pos, Ponto2D_t size) {
  Ponto2D_t pontos[2] = {pos, size};

  Posic_t it = lt_get_first(comercios);

  Lista_t comercios_na_area = lt_create();

  it = lt_search(comercios, it, pontos, compararDentro);
  while (it) {
    lt_insert(comercios_na_area, lt_get(comercios, it));
    it = lt_search(comercios, lt_get_next(comercios, it), pontos, compararDentro);
  }

  return comercios_na_area;
}

/**
 * Comando: tecq?
 * Params:   cep
 * Lista_t os tipos de estabelecimentos comerciais de
 * uma dada quadra.
 * SAIDA: arquivo .txt =>mostrar, para cada tipo de
 * estabelecimento comercial, listar o nome dos
 * estabelecimentos daquele tipo.
 */
int comando_qry_tecq(void *_this, void *_controlador) {
  struct Comando     *this        = _this;
  struct Controlador *controlador = _controlador;

  char *cep = this->params[0];

  HashTable tabela = controlador->tabelas[CEP_X_QUADRA];

  if (!HashTable_t.exists(tabela, cep)) {
    lt_insert(controlador->saida, 
      format_string("O CEP \"%s\" nao foi encontrado.\n", cep));
    return 1;
  }

  lt_insert(controlador->saida, 
    format_string("Comercios na quadra %s:\n", cep));

  Lista_t comercios = controlador->comercios;

  Lista_t comercios_na_quadra = lt_filter(comercios, cep, compararCEP);

  char **tipos = __get_tipos(comercios_na_quadra);

  lt_destroy(comercios_na_quadra, 0);

  if (!tipos) {
    lt_insert(controlador->saida, 
      format_string("Nao há comercios na quadra %s.\n", cep));
    return 1;
  }

  // Passar printando os tipos
  char **tipos_it = tipos;
  while (*tipos_it) {
    char *tipo_desc = HashTable_t.get(controlador->tabelas[TIPO_X_DESCRICAO], *tipos_it);

    lt_insert(controlador->saida, 
      format_string("\t%s:\n", tipo_desc));

    Posic_t it = lt_get_first(comercios);
    it = lt_search(comercios, it, *tipos_it, compararTIPO);
    while (it) {
      Comercio comercio = lt_get(comercios, it);

      char *info_comercio = comercio_get_info(comercio, tipo_desc);

      lt_insert(controlador->saida, 
        format_string("\t\t%s\n", info_comercio));

      free(info_comercio);

      it = lt_search(comercios, lt_get_next(comercios, it), *tipos_it, compararTIPO);
    }

    tipos_it++;
  }

  free(tipos);

  return 1;
}

/**
 * Comando: tecr?
 * Params:  x y larg alt
 * Quais são os tipos de estabelecimentos comerciais
 * existentes numa dada região.
 */
int comando_qry_tecr(void *_this, void *_controlador) {
  struct Comando     *this        = _this;
  struct Controlador *controlador = _controlador;

  __controlador = controlador;

  Ponto2D_t pos  = p2d_new(strtod(this->params[0], 0), strtod(this->params[1], 0));
  Ponto2D_t size = p2d_new(strtod(this->params[2], 0), strtod(this->params[3], 0));

  Lista_t comercios_na_area = __get_comercios_area(controlador->comercios, pos, size);

  char **tipos = __get_tipos(comercios_na_area);

  lt_destroy(comercios_na_area, 0);

  if (!tipos) {
    lt_insert(controlador->saida, 
      format_string("Nao há comercios na regiao (%.1f,%.1f) de tamanho (%.1f,%.1f).\n",
      pos.x, pos.y, size.x, size.y));
    return 1;
  }

  lt_insert(controlador->saida, 
    format_string("Tipos na regiao (%.1f,%.1f) de tamanho (%.1f,%.1f):\n", pos.x, pos.y, size.x, size.y));

  // Printar os tipos na area
  char **tipos_it = tipos;
  while (!!*tipos_it) {
    char *tipo_desc = HashTable_t.get(controlador->tabelas[TIPO_X_DESCRICAO], *tipos_it);

    lt_insert(controlador->saida, 
      format_string("\t%s\n", tipo_desc));

    tipos_it++;
  }

  free(tipos);

  return 1;
}

/**
 * Comando: ecr?
 * Params:  tp [ x y larg alt]
 * Lista_t os estabelecimentos comerciais de um da
 * tipo de, se presente, uma determinada região.
 */
int comando_qry_ecr(void *_this, void *_controlador) {
  struct Comando     *this        = _this;
  struct Controlador *controlador = _controlador;

  __controlador = controlador;

  char *tipo = this->params[0];

  Lista_t comercios;
  Ponto2D_t pos, size;

  if (this->num_param > 1) {
    pos  = p2d_new(strtod(this->params[1], 0), strtod(this->params[2], 0));
    size = p2d_new(strtod(this->params[3], 0), strtod(this->params[4], 0));
    comercios = __get_comercios_area(controlador->comercios, pos, size);
  } else {
    comercios = lt_copy(controlador->comercios);
  }

  char *tipo_desc = HashTable_t.get(controlador->tabelas[TIPO_X_DESCRICAO], tipo);

  // Filtra somente os comercios com o tipo passado
  Lista_t comercios_tipo = lt_filter(comercios, tipo, compararTIPO);

  lt_destroy(comercios, 0);

  if (lt_length(comercios_tipo) == 0) {
    lt_insert(controlador->saida,
      format_string("Nao ha comercios do tipo \"%s\" com os parametros especificados.\n", tipo_desc));
    lt_destroy(comercios_tipo, 0);
    return 1;
  }

  char *saida;

  if (this->num_param == 1)
    saida = format_string("Todos comercios do tipo \"%s\":\n", tipo_desc);
  else
    saida = format_string("Todos comercios do tipo \"%s\" na area (%.0f,%0.f) de tamanho (%.0f,%0.f):\n",
      tipo_desc, pos.x, pos.y, size.x, size.y);

  lt_insert(controlador->saida, saida);

  Posic_t it = lt_get_first(comercios_tipo);
  while (it) {
    Comercio comercio = lt_get(comercios_tipo, it);

    char *info_comercio = comercio_get_info(comercio, tipo_desc);

    lt_insert(controlador->saida, format_string(
      "\t%s\n", info_comercio));

    free(info_comercio);

    it = lt_get_next(comercios_tipo, it);
  }
  
  lt_destroy(comercios_tipo, 0);

  return 1;
}

/**
 * Comando: ecq?
 * Params:   cep
 * Lista_t os estabelecimentos de uma determinada
 * quadra.
 * SAIDA: arquivo .txt =>mostrar cnpj, nome, tipo e
 * endereço de cada estabelecimento comercial.
 */
int comando_qry_ecq(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char *cep = this->params[0];

  Lista_t comercios = controlador->comercios;

  Posic_t it = lt_get_first(comercios);
  it = lt_search(comercios, it, cep, contemCep);

  if (it)
    lt_insert(controlador->saida, 
      format_string("Comercios na quadra %s.\n", cep));

  while (it) {
    Comercio comercio = lt_get(comercios, it);

    char *tipo_desc = HashTable_t.get(controlador->tabelas[TIPO_X_DESCRICAO], comercio_get_tipo(comercio));
    char *info_comercio = comercio_get_info(comercio, tipo_desc);

    lt_insert(controlador->saida, format_string("\t%s\n", info_comercio));

    free(info_comercio);

    it = lt_search(comercios, lt_get_next(comercios, it), cep, contemCep);
  }

  return 1;
}