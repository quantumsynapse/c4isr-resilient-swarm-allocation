# Resultados

## 1. Execução principal

```text
episódios por estratégia             3.000
estratégias                                5
episódios-estratégia                  15.000
agentes por cenário                       18
ações por cenário                          36
semente                                  1995
```

## 2. Médias

| Estratégia | Conclusão informacional | Valor entregue | Continuidade | Pacotes por ação |
|---|---:|---:|---:|---:|
| Central global com RLNC | 0,890417 | 0,892878 | 0,908642 | 10,870093 |
| Gulosa distribuída | 0,535222 | 0,534779 | 0,639594 | 8,053926 |
| Leilão distribuído | 0,495481 | 0,491250 | 0,605916 | 257,261407 |
| Leilão resiliente sem código | 0,727750 | 0,728806 | 0,785981 | 260,190306 |
| Proposta com RLNC adaptativa | 0,886231 | 0,888031 | 0,905228 | 351,892389 |

## 3. Hipótese principal

Diferenças pareadas entre a proposta e o leilão resiliente sem codificação:

| Métrica | Diferença média | IC 95% | t aproximado |
|---|---:|---:|---:|
| Conclusão informacional | 0,158481 | [0,155356; 0,161607] | 99,3817 |
| Valor entregue | 0,159224 | [0,155691; 0,162758] | 88,3174 |
| Continuidade | 0,119246 | [0,116637; 0,121856] | 89,5666 |
| Pacotes transmitidos | 91,702083 | [88,553538; 94,850629] | 57,0854 |

Em termos relativos, a proposta elevou a conclusão informacional em 21,78%, o valor entregue em 21,85% e a continuidade em 15,17%, com aumento de 35,24% na contagem de símbolos-pacote em relação à política robusta sem codificação.

## 4. Robustez a agentes degradados

A aceitação de agentes degradados foi:

```text
leilão distribuído                   0,340559
leilão resiliente sem código         0,007366
proposta com RLNC adaptativa          0,007186
```

A política proposta reduziu essa aceitação em aproximadamente 97,89% em relação ao leilão convencional.

O conflito médio de consenso caiu de 0,043697 na política robusta sem código para 0,034738 na proposta, redução de aproximadamente 20,50%.

## 5. Distância da referência central

A conclusão da proposta ficou 0,004185 abaixo da referência central, diferença de 0,42 ponto percentual.

O valor entregue ficou 0,004848 abaixo da referência central, diferença de 0,48 ponto percentual.

A proximidade não implica equivalência operacional. A referência e a proposta utilizam o mesmo modelo sintético e não representam sistemas reais.

## 6. Perda de pacotes

Conclusão informacional da proposta:

```text
perda 0,05       0,895417
perda 0,15       0,890741
perda 0,25       0,873796
perda 0,35       0,856852
```

Conclusão do leilão resiliente sem código:

```text
perda 0,05       0,740833
perda 0,15       0,714583
perda 0,25       0,709259
perda 0,35       0,718704
```

A proposta manteve vantagem em todos os pontos avaliados. O crescimento de redundância compensou parte da perda, mas elevou a comunicação de 268,21 para 503,78 símbolos-pacote por ação.

## 7. Perturbação de saída

Com três agentes degradados, a aceitação de agentes perturbados pelo modelo proposto foi:

```text
amplitude 0,00       0,108029
amplitude 0,15       0,021252
amplitude 0,30       0,016677
amplitude 0,45       0,017873
```

Com perturbação nula, os agentes degradados são praticamente indistinguíveis dos demais no canal de estado. A partir de amplitude 0,15, o resíduo reduz sua influência.

A conclusão informacional permaneceu entre 0,877083 e 0,881481 nessa série.

## 8. Convergência

```text
episódios      proposta conclusão      robusta conclusão
300            0,886111                0,738333
750            0,884815                0,726963
1.500          0,886778                0,726796
3.000          0,886231                0,727750
```

A média estabilizou sem inversão da ordenação entre a proposta e as referências distribuídas.
