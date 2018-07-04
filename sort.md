---
geometry: margin=1cm
---

## Escolha do Sorting

Foi gerado um caso de teste de torres usando um script em Python. O caso continha aproximadamente 500 mil torres. Após isso, foi executado o algoritmo de procura das menores distâncias com os dois tipos de sorting: merge sort e heap sort.

O merge sort obteve tempos melhores do que o heap sort em todos os casos de teste. Porém, por conta da alocação de memoria necessária durante o merge sort, ele acabava sendo inconsistente durante o uso.

Por isso, foi optado pelo uso do heap sort, que nao apresentou insconsistências durante a execução.