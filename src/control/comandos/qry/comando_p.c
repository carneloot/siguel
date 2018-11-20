// Entrada:  (Pictórico) r1 r2 [cor]

/* Descrição: Qual o melhor trajeto entre a origem que está no registrador r1 e o destino que está no registrador r2.
              Caso o percurso peça a representação pictóricam o trajeto deve ser pintado com a cor especificada
              Atenção: o cálculo do percurso deve levar em consideração as interdições estabelecidas pela última detecção */

// Comando pictórico: (t | p sufixo) (D | T)
/*
    t:          descrição textual               -> Escrever resposta no txt
    p:          representação pictórica         -> Fazer desenho svg
    sufixo:     sufixo de arquivo de saída      -> Nomde do arquivo fica assim: nomeArg-sufixo.svg
    D:          menor distância                 -> Fazer o caminho com menor distância
    T:          menor tempo estimado            -> Fazer o caminho com menor tempo
*/

#include <model/comando.r>
#include <control/controlador.r>

int comando_p(void *_this, void *_controlador) {
  struct Comando *this            = (struct Comando *) _this;
  struct Controlador *controlador = (struct Controlador *) _controlador;

  char** params = this->params;
  
  // parâmetro que diz se quer representação pictória ou textual
  char* eh_pictorico = params[0];

  if( eh_pictorico == "p" ){
    // ================================= É pictórico =================================
    char* sufixo = params[1];

    char* menor_tempo = params[2];
    if( menor_tempo == "T"){
      // TODO ------- Encontrar o caminho de menor tempo ---------------------------
      

      return 1;
    }
    // TODO -------- Encontrar o caminho de menor distância ----------------------------


    return 1;
  }

  // ================================ Não é pictórico, é textual ================================

  // Significa que o próximo parâmetro é o que define se
  //  é para pegar o caminho mais curto ou mais rápido
  char* menor_tempo = params[1];

  if( menor_tempo == "T" ){
    // TODO ---- Procurar o caminho com o menor tempo----------------------------------
    return 1;
  }

  
  // TODO-------Procurar caminho mais curto (menor distância)---------------------------------------
  return 1;
}