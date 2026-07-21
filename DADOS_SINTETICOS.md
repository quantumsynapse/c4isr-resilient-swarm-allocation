# Dados Sintéticos

## 1. Princípio

Todos os valores são gerados pelo programa. Nenhum parâmetro foi extraído de aeronave, sensor, enlace, organização, unidade, local, missão ou sistema real.

As variáveis são normalizadas ou expressas em ciclos abstratos. Os nomes C4ISR descrevem classes funcionais, não especificações operacionais.

## 2. População de referência

A execução de referência gerou:

```text
cenários                                  3.000
agentes por cenário                           18
estados iniciais de agentes               54.000
ações por cenário                              36
ações sintéticas                         108.000
estratégias                                    5
episódios-estratégia                       15.000
```

## 3. Distribuições

Posições de agentes e ações são uniformes no quadrado unitário.

Energia e saúde são médias de duas variáveis uniformes, produzindo distribuições triangulares limitadas. Isso reduz a concentração artificial nas extremidades.

O número de agentes degradados segue distribuição de Poisson com média 1,4 e truncamento em quatro agentes.

O valor informacional segue distribuição lognormal truncada no intervalo [12,120]. A lognormal foi escolhida para representar muitas ações de valor moderado e uma cauda limitada de ações de alto valor.

A perda global é uma transformação limitada de variável uniforme. Cada enlace adiciona um termo quadrático de distância e uma perturbação pequena.

A entrega de cada pacote é um ensaio de Bernoulli condicionado à confiabilidade do caminho. O atraso é limitado e multiplicativo em torno do atraso médio da rota.

## 4. Estatísticas observadas

```text
perda global média                         0,13811535
perda global mínima                        0,02500006
perda global máxima                        0,32477620
agentes degradados médios                  1,35300000
maior componente médio                     0,99885185
energia inicial média                      0,83993971
saúde funcional média                      0,88690502
valor médio das ações                     33,14480056
prazo médio                                38,99256746
complexidade média                         0,59997318
cenários inválidos                                  0
```

## 5. Validade

A validade dos dados é estrutural e experimental:

1. todos os valores obedecem aos limites declarados;
2. todas as capacidades possuem cobertura na população;
3. existem pelo menos três agentes de fusão;
4. não existem NaN ou infinitos;
5. os cenários são reproduzíveis pela semente;
6. a análise de sensibilidade altera perdas e perturbações independentemente.

Esses dados não são uma amostra estatística de uma força aérea, empresa ou teatro de operações. A validade externa deve ser estabelecida separadamente para qualquer aplicação real.
