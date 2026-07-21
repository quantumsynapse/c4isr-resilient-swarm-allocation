#!/bin/sh
set -eu

ARQUIVO=resultados/resumo_monte_carlo.csv
HIPOTESE=resultados/teste_hipotese.csv

obter_media()
{
    estrategia=$1
    metrica=$2
    awk -F, -v e="$estrategia" -v m="$metrica" \
        '$1==e && $2==m {print $3}' "$ARQUIVO"
}

proposta=$(obter_media carma_rlnc_adaptativa conclusao_informacional)
robusta=$(obter_media leilao_resiliente_sem_codigo conclusao_informacional)
leilao=$(obter_media leilao_distribuido conclusao_informacional)
central=$(obter_media central_global_rlnc conclusao_informacional)

awk -v p="$proposta" -v r="$robusta" 'BEGIN {exit !(p > r)}'
awk -v p="$proposta" -v l="$leilao" 'BEGIN {exit !(p > l)}'
awk -v c="$central" -v p="$proposta" 'BEGIN {exit !(c >= p)}'
awk -F, '$1=="proposta_menos_robusta" && \
           $2=="conclusao_informacional" {exit !($5 > 0.0)}' "$HIPOTESE"

echo 'teste_resultados=aprovado'
