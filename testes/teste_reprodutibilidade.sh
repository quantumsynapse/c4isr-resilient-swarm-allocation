#!/bin/sh
set -eu

rm -rf resultados/reprodutibilidade_a resultados/reprodutibilidade_b
mkdir -p resultados/reprodutibilidade_a resultados/reprodutibilidade_b

./simulador_c4isr --episodios 120 --semente 314159 \
    --saida resultados/reprodutibilidade_a --sem-sensibilidade >/dev/null
./simulador_c4isr --episodios 120 --semente 314159 \
    --saida resultados/reprodutibilidade_b --sem-sensibilidade >/dev/null

cmp resultados/reprodutibilidade_a/resumo_monte_carlo.csv \
    resultados/reprodutibilidade_b/resumo_monte_carlo.csv
cmp resultados/reprodutibilidade_a/teste_hipotese.csv \
    resultados/reprodutibilidade_b/teste_hipotese.csv
cmp resultados/reprodutibilidade_a/validacao_dados.txt \
    resultados/reprodutibilidade_b/validacao_dados.txt

echo 'teste_reprodutibilidade=aprovado'
