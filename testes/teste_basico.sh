#!/bin/sh
set -eu

rm -rf resultados/teste
mkdir -p resultados/teste
./simulador_c4isr --episodios 80 --semente 17 \
    --saida resultados/teste --sem-sensibilidade >/dev/null

test -s resultados/teste/resumo_monte_carlo.csv
test -s resultados/teste/teste_hipotese.csv
test -s resultados/teste/validacao_dados.txt
grep -q '^status=aprovado$' resultados/teste/validacao_dados.txt
grep -q '^hipotese_suportada=sim$' resultados/teste/resumo_referencia.txt

echo 'teste_basico=aprovado'
