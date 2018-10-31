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