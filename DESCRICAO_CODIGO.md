# Descrição do Código

## 1. Organização

```text
include/modelo.h
    constantes, tipos e protótipos públicos

src/aleatorio.c
    gerador congruencial, normal de Box-Muller e Poisson

src/util.c
    limites, distância, contagem de bits e nomes

src/grafo.c
    construção do grafo, conectividade e caminho até fusão

src/cenario.c
    geração e validação dos cenários sintéticos

src/gf256.c
    multiplicação, inversão, posto e simulação RLNC

src/simulacao.c
    ofertas, confiança, leilão, coalizões e execução das ações

src/metricas.c
    acumuladores e intervalos de confiança

src/experimento.c
    Monte Carlo, sensibilidade e gravação de resultados

src/principal.c
    interface de linha de comando
```

## 2. Convenções

O programa utiliza C89/C90, comentários tradicionais, armazenamento estático de dimensão limitada e arquivos de texto simples.

Não há threads, sockets, bibliotecas externas, serialização binária, banco de dados, acesso à rede ou execução de comandos pelo programa C.

## 3. Aritmética finita

`gf256_multiplicar` executa multiplicação polinomial com redução por 0x11d.

`gf256_inverter` usa exponenciação a 254.

`gf256_posto` executa eliminação de Gauss-Jordan sobre GF(2^8).

`transmissao_rlnc` cria linhas sistemáticas seguidas por linhas aleatórias, remove pacotes apagados ou atrasados e verifica o posto.

## 4. Complexidade

Com \(N\) agentes, \(T\) ações e \(R\) rodadas, a propagação do leilão é limitada por

\[
O(T R N^2).
\]

O caminho até fusão usa Dijkstra matricial, com custo \(O(N^2)\) por consulta.

A eliminação em uma janela de \(K\leq12\) símbolos custa \(O(K^3)\), limitada por constante pequena na configuração de referência.

## 5. Entradas

```text
--episodios N
--semente N
--saida DIRETORIO
--sem-sensibilidade
--perda P
--degradados N
--perturbacao A
```

## 6. Saídas

```text
resumo_monte_carlo.csv
teste_hipotese.csv
convergencia.csv
sensibilidade_perda.csv
sensibilidade_perturbacao.csv
validacao_dados.txt
resumo_referencia.txt
```
