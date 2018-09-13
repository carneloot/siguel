#include <comando.r>
#include <controlador.r>

#include <stdlib.h>
#include <string.h>
#include <pessoa.h>
#include <desenhavel.h>
#include <endereco.h>
#include <utils.h>

#define SVG_FIGURA_SETA "<svg viewBox=\"0 0 451.846 451.847\" style=\"enable-background:new 0 0 451.846 451.847;\" xml:space=\"preserve\"><g transform=\"rotate(%.1f 225.923 225.923)\"><path d=\"M345.441,248.292L151.154,442.573c-12.359,12.365-32.397,12.365-44.75,0c-12.354-12.354-12.354-32.391,0-44.744		L278.318,225.92L106.409,54.017c-12.354-12.359-12.354-32.394,0-44.748c12.354-12.359,32.391-12.359,44.75,0l194.287,194.284 c6.177,6.18,9.262,14.271,9.262,22.366C354.708,234.018,351.617,242.115,345.441,248.292z\"/></g></svg>"

static char *svg_pontos(void *_pontos) {
  Ponto2D *pontos = _pontos;
  Ponto2D a = pontos[0];
  Ponto2D b = pontos[1];

  Ponto2D pSeta = Ponto2D_t.add(a, b);
  pSeta = Ponto2D_t.mult(pSeta, 0.5);
  pSeta = Ponto2D_t.sub_scalar(pSeta, 10);

  double angulo = rad_to_deg(Ponto2D_t.angle(Ponto2D_t.sub(b, a)));

  char *svg_seta = format_string(SVG_FIGURA_SETA, angulo);

  char *seta = format_string(
    "<svg style=\"fill:%s;opacity:0.8;\" width=\"%.2f\" height=\"%.2f\" x=\"%.2f\" y=\"%.2f\">\n%s</svg>",
    "green",
    20.0, 20.0,
    pSeta.x, pSeta.y, svg_seta
  );

  free(svg_seta);
  
  char *linha = format_string(
    "<line x1=\"%.1f\" y1=\"%.1f\" x2=\"%.1f\" y2=\"%.1f\" "
    "style=\"stroke:%s;stroke-width:4;opacity:0.8\" />",
    a.x, a.y,
    b.x, b.y,
    "green"
  );

  char *saida = format_string(
    "%s\n%s\n", linha, seta);

  free(linha);
  free(seta);

  return saida;
}

int __comando_qry_mud(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char *cpf = this->params[0];

  HashTable tabela_pessoa = controlador->tabelas[CPF_X_PESSOA];
  HashTable tabela_cep    = controlador->tabelas[CPF_X_CEP];

  if (!HashTable_t.exists(tabela_pessoa, cpf)) {
    char *saida = format_string(
      "A pessoa com o CPF \"%s\" nao foi encontrada.\n", cpf);
    Lista_t.insert(controlador->saida, saida);
    return 1;
  } 

  HashInfo hash_info = HashTable_t.get(tabela_pessoa, cpf);
  Pessoa pessoa = hash_info.valor;

  char *cep   = this->params[1];
  int face    = char_to_face(this->params[2][0]);
  int numero  = strtol(this->params[3], NULL, 10);
  char *compl = this->params[4];

  Ponto2D pos_antiga = endereco_get_coordenada(pessoa_get_endereco(pessoa), controlador);
  char *info_pessoa = pessoa_get_info(pessoa, controlador);

  HashTable_t.remove(tabela_cep, cpf);
  pessoa_set_endereco(pessoa, cep, face, numero, compl);

  HashInfo new_info = {
    .chave = pessoa_get_cpf(pessoa),
    .valor = pessoa_get_cep(pessoa)
  };
  HashTable_t.insert(tabela_cep, new_info);

  Ponto2D pos_atual = endereco_get_coordenada(pessoa_get_endereco(pessoa), controlador);
  char *info_endereco_atual = endereco_get_info(pessoa_get_endereco(pessoa));

  char *saida = format_string(
    "Mudanca de endereco:\n%s\nmudou para %s\n",
    info_pessoa, info_endereco_atual);
  Lista_t.insert(controlador->saida, saida);

  free(info_pessoa);
  free(info_endereco_atual);

  // Desenhar uma linha de pos_antiga ate pos_atual
  Ponto2D *pontos = calloc(2, sizeof(*pontos));
  pontos[0] = pos_antiga;
  pontos[1] = pos_atual;

  Ponto2D new_max = Ponto2D_t.maximo(pos_antiga, pos_atual);
  controlador->max_qry = Ponto2D_t.maximo(controlador->max_qry, new_max);

  Lista_t.insert(controlador->saida_svg_qry, 
    cria_desenhavel(pontos, svg_pontos, free)
  );
  
  return 1;
}