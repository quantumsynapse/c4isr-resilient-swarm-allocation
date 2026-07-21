#include <math.h>
#include <string.h>
#include "modelo.h"

void acumulador_inicializar(Acumulador *a)
{
    a->soma = 0.0;
    a->soma_quadrados = 0.0;
    a->n = 0L;
}

void acumulador_adicionar(Acumulador *a, double valor)
{
    a->soma += valor;
    a->soma_quadrados += valor * valor;
    a->n++;
}

ResumoEstatistico acumulador_resumir(const Acumulador *a)
{
    ResumoEstatistico r;
    double variancia;

    memset(&r, 0, sizeof(r));
    if (a->n <= 0L) {
        return r;
    }
    r.media = a->soma / (double)a->n;
    if (a->n > 1L) {
        variancia = (a->soma_quadrados -
                    a->soma * a->soma / (double)a->n) /
                    (double)(a->n - 1L);
        if (variancia < 0.0 && variancia > -1.0e-12) {
            variancia = 0.0;
        }
        if (variancia > 0.0) {
            r.desvio = sqrt(variancia);
        }
    }
    r.erro_padrao = r.desvio / sqrt((double)a->n);
    r.limite_inferior = r.media - 1.96 * r.erro_padrao;
    r.limite_superior = r.media + 1.96 * r.erro_padrao;
    return r;
}

void estatistica_inicializar(EstatisticaEstrategia *e)
{
    acumulador_inicializar(&e->execucao_fisica);
    acumulador_inicializar(&e->conclusao_informacional);
    acumulador_inicializar(&e->valor_entregue);
    acumulador_inicializar(&e->cumprimento_prazo);
    acumulador_inicializar(&e->energia_consumida);
    acumulador_inicializar(&e->pacotes_transmitidos);
    acumulador_inicializar(&e->conflitos_consenso);
    acumulador_inicializar(&e->reatribuicoes);
    acumulador_inicializar(&e->aceitacoes_degradadas);
    acumulador_inicializar(&e->conectividade);
    acumulador_inicializar(&e->atraso_decodificacao);
    acumulador_inicializar(&e->continuidade);
}

void estatistica_adicionar(EstatisticaEstrategia *e,
                           const MetricaEpisodio *m)
{
    acumulador_adicionar(&e->execucao_fisica, m->execucao_fisica);
    acumulador_adicionar(&e->conclusao_informacional,
                         m->conclusao_informacional);
    acumulador_adicionar(&e->valor_entregue, m->valor_entregue);
    acumulador_adicionar(&e->cumprimento_prazo, m->cumprimento_prazo);
    acumulador_adicionar(&e->energia_consumida, m->energia_consumida);
    acumulador_adicionar(&e->pacotes_transmitidos,
                         m->pacotes_transmitidos);
    acumulador_adicionar(&e->conflitos_consenso,
                         m->conflitos_consenso);
    acumulador_adicionar(&e->reatribuicoes, m->reatribuicoes);
    acumulador_adicionar(&e->aceitacoes_degradadas,
                         m->aceitacoes_degradadas);
    acumulador_adicionar(&e->conectividade, m->conectividade);
    acumulador_adicionar(&e->atraso_decodificacao,
                         m->atraso_decodificacao);
    acumulador_adicionar(&e->continuidade, m->continuidade);
}
