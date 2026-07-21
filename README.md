# Alocação C4ISR Distribuída e Resiliente para Enxames UAS com RLNC Adaptativa

## 1. Finalidade

Este projeto implementa, em C89/C90, uma simulação integralmente sintética de alocação distribuída de ações para veículos aéreos não tripulados representados como agentes orientados a objetivos.

O domínio considerado é C4ISR não cinético. As ações abstratas representam observação, reconhecimento, retransmissão, mapeamento, confirmação multissensorial e fusão de informação. O programa não contém seleção de alvos, emprego de armamentos, parâmetros reais de plataformas, frequências, enlaces, sensores, locais ou missões.

A hipótese avaliada é a seguinte:

> O acoplamento entre alocação distribuída, confiança residual, consciência da topologia e codificação linear aleatória adaptativa aumenta a conclusão informacional de ações sob perdas de comunicação e perturbações de saída.

## 2. Estrutura técnica

O modelo combina:

1. grafo dinâmico de comunicação;
2. agentes heterogêneos e orientados a objetivos;
3. leilão distribuído com propagação local de ofertas;
4. filtragem de ofertas por confiança e plausibilidade;
5. perturbação sintética de saídas de estado;
6. escolha de fonte e rota até um nó de fusão;
7. Random Linear Network Coding sobre GF(2^8);
8. redundância adaptativa condicionada à perda, prioridade e confiança;
9. Monte Carlo pareado, intervalos de confiança e análise de sensibilidade.

A aritmética em GF(2^8) utiliza o polinômio irredutível

```text
x^8 + x^4 + x^3 + x^2 + 1
```

conforme a especificação do RFC 8681. A implementação modela os coeficientes, a recepção dos símbolos e o posto da matriz de decodificação. Ela não implementa o formato de pacotes do RFC.

## 3. Estratégias comparadas

```text
central_global_rlnc
    Referência centralizada com informação global e RLNC.

gulosa_distribuida
    Seleção local por proximidade, sem consenso e sem codificação.

leilao_distribuido
    Leilão local sem filtragem robusta e comunicação não codificada.

leilao_resiliente_sem_codigo
    Leilão com confiança residual e consciência da comunicação,
    mas transmissão dos dados por repetição não codificada.

carma_rlnc_adaptativa
    Modelo proposto: leilão resiliente, comunicação consciente da
    topologia e RLNC adaptativa no plano de dados.
```

## 4. Compilação

Requisitos:

```text
Debian GNU/Linux ou sistema POSIX equivalente
gcc
make
libm
shell POSIX
```

Compilação estrita:

```bash
gcc -std=c89 -pedantic -Wall -Wextra -O2
```

Execução completa:

```bash
make validar
```

Execução direta:

```bash
make
mkdir -p resultados
./simulador_c4isr --episodios 3000 --semente 1995 --saida resultados
```

## 5. Resultados de referência

A execução de referência utilizou 3.000 cenários comuns às cinco estratégias, totalizando 15.000 episódios-estratégia, 54.000 estados iniciais de agentes e 108.000 ações sintéticas.

```text
Estratégia                         Conclusão      Valor          Continuidade
central_global_rlnc                0,890417       0,892878       0,908642
gulosa_distribuida                 0,535222       0,534779       0,639594
leilao_distribuido                 0,495481       0,491250       0,605916
leilao_resiliente_sem_codigo       0,727750       0,728806       0,785981
carma_rlnc_adaptativa              0,886231       0,888031       0,905228
```

Em relação ao leilão resiliente sem codificação, o modelo proposto obteve:

```text
ganho absoluto em conclusão informacional     0,158481
ganho relativo em conclusão informacional     21,78%
ganho absoluto em valor entregue              0,159224
ganho relativo em valor entregue              21,85%
ganho relativo no índice de continuidade      15,17%
aumento de símbolos-pacote transmitidos        35,24%
```

O intervalo de confiança de 95% da diferença pareada em conclusão informacional foi [0,155356; 0,161607].

## 6. Limites

Os resultados demonstram propriedades do simulador sob as distribuições e restrições documentadas. Eles não demonstram desempenho em voo, conformidade militar, segurança criptográfica, resistência a interferência eletromagnética real ou adequação operacional.

A codificação de apagamentos não oferece confidencialidade, autenticidade ou integridade. Esses requisitos devem ser providos por mecanismos criptográficos padronizados e independentes.

## 7. Arquivos principais

```text
MODELO.md
METODOLOGIA.md
DADOS_SINTETICOS.md
DESCRICAO_CODIGO.md
ANALISE_TECNICO_CIENTIFICA.md
RESULTADOS.md
RELATORIO_VALIDACAO.md
SEGURANCA_E_LIMITES.md
REFERENCIAS.md
```
