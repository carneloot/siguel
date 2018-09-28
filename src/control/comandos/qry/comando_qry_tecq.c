#include <model/comando.r>
#include <control/controlador.r>

#include <string.h>
#include <stdlib.h>

#include <model/comercio.h>
#include <model/figura.h>
#include <model/utils.h>

static void *__controlador = NULL;

static int compararDentro(void * const comercio, const void *outro) {
  const Ponto2D *pontos = outro;
  Ponto2D pos  = pontos[0];
  Ponto2D size = pontos[1];

  Ponto2D pos_comercio = endereco_get_coordenada(comercio_get_endereco(comercio), __controlador);

  Figura ret = cria_retangulo(pos.x, pos.y, size.x, size.y, "", "");

  int result = contem_ponto(ret, pos_comercio);

  destruir_figura(ret);
  
  return !result;
}

static int compararCEP(void * const comercio, const void *outro) {
  return strcmp(comercio_get_endereco(comercio)->cep, outro);
}

static int compararTIPO(void * const comercio, const void *outro) {
  return strcmp(comercio_get_tipo(comercio), outro);
}

char **__get_tipos(Lista comercios) {
  size_t tam = Lista_t.length(comercios);

  if (tam == 0) return NULL;

  char **tipos = calloc(tam + 1, sizeof(*tipos));

  // Peguei todos os tipos
  Posic it = Lista_t.get_first(comercios);
  while (it) {
    Comercio comercio = Lista_t.get(comercios, it);
    char *tipo_atual = comercio_get_tipo(comercio);

    char **tipos_it = tipos;

    // Aumenta o tipos_it até ficar vazio ou achar um igual
    while (*tipos_it != NULL && strcmp(tipo_atual, *tipos_it++));
    *tipos_it = tipo_atual;

    it = Lista_t.get_next(comercios, it);
  }

  return tipos;
}

Lista __get_comercios_area(Lista comercios, Ponto2D pos, Ponto2D size) {
  Ponto2D pontos[2] = {pos, size};

  Posic it = Lista_t.get_first(comercios);

  Lista comercios_na_area = Lista_t.create();

  it = Lista_t.search(comercios, it, pontos, compararDentro);
  while (it) {
    Lista_t.insert(comercios_na_area, Lista_t.get(comercios, it));
    it = Lista_t.search(comercios, Lista_t.get_next(comercios, it), pontos, compararDentro);
  }

  return comercios_na_area;
}

int comando_qry_tecq(void *_this, void *_controlador) {
  struct Comando     *this        = _this;
  struct Controlador *controlador = _controlador;

  char *cep = this->params[0];

  HashTable tabela = controlador->tabelas[CEP_X_QUADRA];

  if (!HashTable_t.exists(tabela, cep)) {
    Lista_t.insert(controlador->saida, 
      format_string("O CEP \"%s\" nao foi encontrado.\n", cep));
    return 1;
  }

  Lista_t.insert(controlador->saida, 
    format_string("Comercios na quadra %s:\n", cep));

  Lista comercios = controlador->comercios;

  Lista comercios_na_quadra = Lista_t.filter(comercios, cep, compararCEP);

  char **tipos = __get_tipos(comercios_na_quadra);

  Lista_t.destruir(comercios_na_quadra, 0);

  if (!tipos) {
    Lista_t.insert(controlador->saida, 
      format_string("Nao há comercios na quadra %s.\n", cep));
    return 1;
  }

  // Passar printando os tipos
  char **tipos_it = tipos;
  while (*tipos_it) {
    char *tipo_desc = HashTable_t.get(controlador->tabelas[TIPO_X_DESCRICAO], *tipos_it).valor;

    Lista_t.insert(controlador->saida, 
      format_string("\t%s:\n", tipo_desc));

    Posic it = Lista_t.get_first(comercios);
    it = Lista_t.search(comercios, it, *tipos_it, compararTIPO);
    while (it) {
      Comercio comercio = Lista_t.get(comercios, it);

      char *info_comercio = comercio_get_info(comercio, tipo_desc);

      Lista_t.insert(controlador->saida, 
        format_string("\t\t%s\n", info_comercio));

      free(info_comercio);

      it = Lista_t.search(comercios, Lista_t.get_next(comercios, it), *tipos_it, compararTIPO);
    }

    tipos_it++;
  }

  free(tipos);

  return 1;
}

int comando_qry_tecr(void *_this, void *_controlador) {
  struct Comando     *this        = _this;
  struct Controlador *controlador = _controlador;

  __controlador = controlador;

  Ponto2D pos  = Ponto2D_t.new(strtod(this->params[0], 0), strtod(this->params[1], 0));
  Ponto2D size = Ponto2D_t.new(strtod(this->params[2], 0), strtod(this->params[3], 0));

  Lista comercios_na_area = __get_comercios_area(controlador->comercios, pos, size);

  char **tipos = __get_tipos(comercios_na_area);

  Lista_t.destruir(comercios_na_area, 0);

  if (!tipos) {
    Lista_t.insert(controlador->saida, 
      format_string("Nao há comercios na regiao (%.1f,%.1f) de tamanho (%.1f,%.1f).\n",
      pos.x, pos.y, size.x, size.y));
    return 1;
  }

  Lista_t.insert(controlador->saida, 
    format_string("Tipos na regiao (%.1f,%.1f) de tamanho (%.1f,%.1f):\n", pos.x, pos.y, size.x, size.y));

  // Printar os tipos na area
  char **tipos_it = tipos;
  while (!!*tipos_it) {
    char *tipo_desc = HashTable_t.get(controlador->tabelas[TIPO_X_DESCRICAO], *tipos_it).valor;

    Lista_t.insert(controlador->saida, 
      format_string("\t%s\n", tipo_desc));

    tipos_it++;
  }

  free(tipos);

  return 1;
}

int comando_qry_ecr(void *_this, void *_controlador) {
  struct Comando     *this        = _this;
  struct Controlador *controlador = _controlador;

  __controlador = controlador;

  char *tipo = this->params[0];

  Lista comercios;
  Ponto2D pos, size;

  if (this->num_param > 1) {
    pos  = Ponto2D_t.new(strtod(this->params[1], 0), strtod(this->params[2], 0));
    size = Ponto2D_t.new(strtod(this->params[3], 0), strtod(this->params[4], 0));
    comercios = __get_comercios_area(controlador->comercios, pos, size);
  } else {
    comercios = Lista_t.copy(controlador->comercios);
  }

  char *tipo_desc = HashTable_t.get(controlador->tabelas[TIPO_X_DESCRICAO], tipo).valor;

  // Filtra somente os comercios com o tipo passado
  Lista comercios_tipo = Lista_t.filter(comercios, tipo, compararTIPO);

  Lista_t.destruir(comercios, 0);

  if (Lista_t.length(comercios_tipo) == 0) {
    Lista_t.insert(controlador->saida,
      format_string("Nao ha comercios do tipo \"%s\" com os parametros especificados.\n", tipo_desc));
    Lista_t.destruir(comercios_tipo, 0);
    return 1;
  }

  char *saida;

  if (this->num_param == 1)
    saida = format_string("Todos comercios do tipo \"%s\":\n", tipo_desc);
  else
    saida = format_string("Todos comercios do tipo \"%s\" na area (%.0f,%0.f) de tamanho (%.0f,%0.f):\n",
      tipo_desc, pos.x, pos.y, size.x, size.y);

  Lista_t.insert(controlador->saida, saida);

  Posic it = Lista_t.get_first(comercios_tipo);
  while (it) {
    Comercio comercio = Lista_t.get(comercios_tipo, it);

    char *info_comercio = comercio_get_info(comercio, tipo_desc);

    Lista_t.insert(controlador->saida, format_string(
      "\t%s\n", info_comercio));

    free(info_comercio);

    it = Lista_t.get_next(comercios_tipo, it);
  }
  
  Lista_t.destruir(comercios_tipo, 0);

  return 1;
}
