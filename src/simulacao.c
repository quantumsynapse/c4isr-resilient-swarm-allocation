#include <math.h>
#include <string.h>
#include "modelo.h"

#define MENOS_INFINITO (-1.0e30)
#define RODADAS_LEILAO 4

static double cobertura_capacidades(const Agente *agente, const Acao *acao)
{
    int requeridas;
    int atendidas;

    requeridas = contar_bits(acao->capacidades_requeridas);
    atendidas = contar_bits(agente->capacidades &
                            acao->capacidades_requeridas);
    if (requeridas <= 0) {
        return 0.0;
    }
    return (double)atendidas / (double)requeridas;
}

static int candidato_primario(const Agente *agente, const Acao *acao)
{
    double cobertura;

    if (agente->energia < 0.08 || agente->saude < 0.50) {
        return 0;
    }
    cobertura = cobertura_capacidades(agente, acao);
    if (acao->agentes_minimos == 1) {
        return cobertura >= 0.999;
    }
    return cobertura > 0.0;
}

static double pontuacao_base(const Cenario *cenario, const Acao *acao,
                             int indice, double energia_observada,
                             double saude_observada,
                             int consciente_comunicacao)
{
    const Agente *agente;
    double cobertura;
    double d;
    double probabilidade;
    double atraso;
    double centralidade;
    double pontuacao;
    int saltos;

    agente = &cenario->agentes[indice];
    if (!candidato_primario(agente, acao)) {
        return MENOS_INFINITO;
    }
    cobertura = cobertura_capacidades(agente, acao);
    d = distancia(agente->x, agente->y, acao->x, acao->y);
    probabilidade = 0.0;
    atraso = 100.0;
    saltos = 0;
    if (consciente_comunicacao) {
        melhor_caminho_fusao(cenario, indice, &probabilidade, &atraso,
                             &saltos);
    }
    centralidade = centralidade_grau(cenario, indice);

    pontuacao = 1.15 * acao->valor * acao->prioridade * cobertura
                * limitar(saude_observada, 0.0, 1.0)
                + 18.0 * agente->vies_objetivo
                - 34.0 * d
                - 18.0 * agente->carga
                - 22.0 * (1.0 - limitar(energia_observada, 0.0, 1.0));

    if (consciente_comunicacao) {
        pontuacao += 34.0 * probabilidade - 0.40 * atraso;
        if ((agente->capacidades & CAP_RELE) != 0U) {
            pontuacao -= 10.0 * centralidade;
        }
    }
    return pontuacao;
}

static double atualizar_confianca(Agente *agente, Gerador *g,
                                  double *energia_reportada,
                                  double *saude_reportada)
{
    double ruido;
    double perturbacao;
    double residual;
    double alvo;
    double sigma;

    ruido = 0.018 * rng_normal(g);
    perturbacao = ruido;
    if (agente->degradado) {
        perturbacao += agente->perturbacao_persistente
                       + 0.04 * rng_normal(g);
    }
    *energia_reportada = limitar(agente->energia + perturbacao, 0.0, 1.25);
    *saude_reportada = limitar(agente->saude + 0.80 * perturbacao,
                               0.0, 1.25);
    residual = fabs(*energia_reportada - agente->energia)
               + fabs(*saude_reportada - agente->saude);
    sigma = 0.12;
    alvo = exp(-(residual * residual) / (2.0 * sigma * sigma));
    agente->confianca = limitar(0.88 * agente->confianca + 0.12 * alvo,
                                0.0, 1.0);
    return perturbacao;
}

static double oferta_reportada(Cenario *cenario, const Acao *acao,
                               int indice, int robusta, Gerador *g)
{
    Agente *agente;
    double energia_reportada;
    double saude_reportada;
    double oferta;
    double limite_superior;
    double perturbacao;

    agente = &cenario->agentes[indice];
    perturbacao = atualizar_confianca(agente, g, &energia_reportada,
                                      &saude_reportada);
    oferta = pontuacao_base(cenario, acao, indice, energia_reportada,
                            saude_reportada, robusta);
    if (oferta <= MENOS_INFINITO / 2.0) {
        return MENOS_INFINITO;
    }
    if (agente->degradado && !robusta) {
        oferta += fabs(perturbacao) * acao->valor * 2.2 + 18.0;
    }
    if (robusta) {
        limite_superior = 1.55 * acao->valor + 55.0;
        oferta = limitar(oferta, -60.0, limite_superior);
        oferta *= 0.20 + 0.80 * agente->confianca;
    }
    return oferta;
}

static int selecionar_central(const Cenario *cenario, const Acao *acao)
{
    int i;
    int melhor;
    double oferta;
    double melhor_oferta;

    melhor = -1;
    melhor_oferta = MENOS_INFINITO;
    for (i = 0; i < cenario->numero_agentes; i++) {
        oferta = pontuacao_base(cenario, acao, i,
                                cenario->agentes[i].energia,
                                cenario->agentes[i].saude, 1);
        if (cenario->agentes[i].degradado) {
            oferta -= 42.0;
        }
        if (oferta > melhor_oferta) {
            melhor_oferta = oferta;
            melhor = i;
        }
    }
    return melhor;
}

static int selecionar_guloso(const Cenario *cenario, const Acao *acao)
{
    int i;
    int melhor;
    double d;
    double melhor_d;

    melhor = -1;
    melhor_d = 1.0e30;
    for (i = 0; i < cenario->numero_agentes; i++) {
        if (!candidato_primario(&cenario->agentes[i], acao)) {
            continue;
        }
        d = distancia(cenario->agentes[i].x, cenario->agentes[i].y,
                      acao->x, acao->y);
        d += 0.35 * cenario->agentes[i].carga;
        if (d < melhor_d) {
            melhor_d = d;
            melhor = i;
        }
    }
    return melhor;
}

static int selecionar_leilao(Cenario *cenario, const Acao *acao,
                             int robusta, int codificada, Gerador *g,
                             double *conflito, double *pacotes_controle)
{
    int vencedor[MAX_AGENTES];
    int vencedor_novo[MAX_AGENTES];
    double oferta[MAX_AGENTES];
    double oferta_nova[MAX_AGENTES];
    int votos[MAX_AGENTES];
    int ativo[MAX_AGENTES];
    int ativo_novo[MAX_AGENTES];
    int i;
    int j;
    int r;
    int elegiveis;
    int melhor;
    int max_votos;
    double p_sucesso;
    double perda;
    double fator_pacotes;
    int copias_controle;

    for (i = 0; i < cenario->numero_agentes; i++) {
        oferta[i] = oferta_reportada(cenario, acao, i, robusta, g);
        if (oferta[i] > MENOS_INFINITO / 2.0) {
            vencedor[i] = i;
            ativo[i] = 1;
        } else {
            vencedor[i] = -1;
            ativo[i] = 0;
        }
    }

    fator_pacotes = 1.0;
    for (r = 0; r < RODADAS_LEILAO; r++) {
        for (i = 0; i < cenario->numero_agentes; i++) {
            vencedor_novo[i] = vencedor[i];
            oferta_nova[i] = oferta[i];
            ativo_novo[i] = 0;
        }
        for (i = 0; i < cenario->numero_agentes; i++) {
            if (vencedor[i] < 0 || !ativo[i]) {
                continue;
            }
            for (j = 0; j < cenario->numero_agentes; j++) {
                if (!cenario->grafo.adj[i][j]) {
                    continue;
                }
                perda = cenario->grafo.perda[i][j];
                p_sucesso = 1.0 - perda;
                copias_controle = 1;
                if (codificada &&
                    (perda > 0.28 ||
                     (acao->prioridade > 0.88 && perda > 0.15))) {
                    copias_controle = 2;
                    p_sucesso = 1.0 - perda * perda;
                }
                fator_pacotes = (double)copias_controle;
                *pacotes_controle += fator_pacotes;
                if (rng_uniforme(g) > p_sucesso) {
                    continue;
                }
                if (oferta[i] > oferta_nova[j] + 1.0e-12 ||
                    (fabs(oferta[i] - oferta_nova[j]) <= 1.0e-12 &&
                     vencedor[i] >= 0 &&
                     (vencedor_novo[j] < 0 ||
                      vencedor[i] < vencedor_novo[j]))) {
                    vencedor_novo[j] = vencedor[i];
                    oferta_nova[j] = oferta[i];
                    ativo_novo[j] = 1;
                }
            }
        }
        for (i = 0; i < cenario->numero_agentes; i++) {
            vencedor[i] = vencedor_novo[i];
            oferta[i] = oferta_nova[i];
            ativo[i] = ativo_novo[i];
        }
    }

    for (i = 0; i < cenario->numero_agentes; i++) {
        votos[i] = 0;
    }
    elegiveis = 0;
    for (i = 0; i < cenario->numero_agentes; i++) {
        if (robusta && cenario->agentes[i].confianca < 0.30) {
            continue;
        }
        elegiveis++;
        if (vencedor[i] >= 0 && vencedor[i] < cenario->numero_agentes) {
            votos[vencedor[i]]++;
        }
    }

    melhor = -1;
    max_votos = 0;
    for (i = 0; i < cenario->numero_agentes; i++) {
        if (votos[i] > max_votos) {
            max_votos = votos[i];
            melhor = i;
        }
    }
    if (elegiveis <= 0) {
        *conflito = 1.0;
        return -1;
    }
    *conflito = 1.0 - (double)max_votos / (double)elegiveis;
    if (max_votos < (int)(0.42 * (double)elegiveis + 0.999)) {
        return -1;
    }
    return melhor;
}

static int selecionar_parceiro(const Cenario *cenario, const Acao *acao,
                               int primario, int consciente_comunicacao)
{
    int i;
    int melhor;
    unsigned int uniao;
    double oferta;
    double melhor_oferta;
    double d;

    if (acao->agentes_minimos <= 1) {
        return -1;
    }
    melhor = -1;
    melhor_oferta = MENOS_INFINITO;
    for (i = 0; i < cenario->numero_agentes; i++) {
        if (i == primario || cenario->agentes[i].energia < 0.08 ||
            cenario->agentes[i].saude < 0.50) {
            continue;
        }
        uniao = cenario->agentes[primario].capacidades |
                cenario->agentes[i].capacidades;
        if ((uniao & acao->capacidades_requeridas) !=
            acao->capacidades_requeridas) {
            if ((cenario->agentes[i].capacidades & CAP_RELE) == 0U) {
                continue;
            }
        }
        d = distancia(cenario->agentes[primario].x,
                      cenario->agentes[primario].y,
                      cenario->agentes[i].x,
                      cenario->agentes[i].y);
        oferta = 0.65 * acao->valor * acao->prioridade
                 * (0.5 + 0.5 * cenario->agentes[i].saude)
                 + 18.0 * cenario->agentes[i].energia
                 - 16.0 * d
                 - 12.0 * cenario->agentes[i].carga;
        if (consciente_comunicacao) {
            double probabilidade;
            double atraso;
            int saltos;
            melhor_caminho_fusao(cenario, i, &probabilidade, &atraso,
                                 &saltos);
            oferta += 24.0 * probabilidade - 0.25 * atraso;
        }
        if (oferta > melhor_oferta) {
            melhor_oferta = oferta;
            melhor = i;
        }
    }
    return melhor;
}

static double cobertura_coalizao(const Cenario *cenario, const Acao *acao,
                                 int primario, int parceiro)
{
    unsigned int uniao;
    int requeridas;
    int atendidas;

    uniao = cenario->agentes[primario].capacidades;
    if (parceiro >= 0) {
        uniao |= cenario->agentes[parceiro].capacidades;
    }
    requeridas = contar_bits(acao->capacidades_requeridas);
    atendidas = contar_bits(uniao & acao->capacidades_requeridas);
    if (requeridas <= 0) {
        return 0.0;
    }
    return (double)atendidas / (double)requeridas;
}

static int executar_acao(Cenario *cenario, const Acao *acao,
                         int estrategia, int primario, int parceiro,
                         Gerador *g, double *energia_gasta,
                         double *pacotes_dados, double *atraso_decodificacao)
{
    Agente *a;
    Agente *b;
    int fonte;
    double d1;
    double d2;
    double dmax;
    double saude_media;
    double cobertura;
    double tempo_execucao;
    double custo1;
    double custo2;
    double prob_execucao;
    double prob1;
    double prob2;
    double atraso1;
    double atraso2;
    int saltos1;
    int saltos2;
    double prob_caminho;
    double atraso_caminho;
    int saltos;
    double prazo_restante;
    double redundancia;
    int transmissoes;
    int recebidos;
    int decodificado;
    double atraso_obs;
    double confianca_fonte;

    a = &cenario->agentes[primario];
    b = NULL;
    if (parceiro >= 0) {
        b = &cenario->agentes[parceiro];
    }
    d1 = distancia(a->x, a->y, acao->x, acao->y);
    d2 = 0.0;
    if (b != NULL) {
        d2 = distancia(b->x, b->y, acao->x, acao->y);
    }
    dmax = d1;
    if (d2 > dmax) {
        dmax = d2;
    }
    saude_media = a->saude;
    if (b != NULL) {
        saude_media = 0.5 * (saude_media + b->saude);
    }
    cobertura = cobertura_coalizao(cenario, acao, primario, parceiro);
    tempo_execucao = 3.0 + 12.0 * dmax /
                     (0.72 + 0.28 * saude_media)
                     + 7.0 * acao->complexidade;
    if (b != NULL) {
        tempo_execucao += 1.5;
    }
    if (tempo_execucao >= acao->prazo) {
        return 0;
    }

    custo1 = 0.012 + 0.050 * d1 + 0.024 * acao->complexidade;
    custo2 = 0.0;
    if (b != NULL) {
        custo2 = 0.010 + 0.042 * d2 + 0.018 * acao->complexidade;
    }
    if (a->energia < custo1 || (b != NULL && b->energia < custo2)) {
        return 0;
    }
    a->energia -= custo1;
    a->carga += 0.030 + 0.020 * acao->complexidade;
    *energia_gasta += custo1;
    if (b != NULL) {
        b->energia -= custo2;
        b->carga += 0.022 + 0.016 * acao->complexidade;
        *energia_gasta += custo2;
    }

    prob_execucao = (0.76 + 0.20 * saude_media) * cobertura;
    if (a->degradado) {
        prob_execucao -= 0.10;
    }
    if (b != NULL && b->degradado) {
        prob_execucao -= 0.06;
    }
    prob_execucao = limitar(prob_execucao, 0.15, 0.995);
    if (rng_uniforme(g) > prob_execucao) {
        return 0;
    }

    melhor_caminho_fusao(cenario, primario, &prob1, &atraso1, &saltos1);
    fonte = primario;
    prob_caminho = prob1;
    atraso_caminho = atraso1;
    saltos = saltos1;
    if (b != NULL) {
        melhor_caminho_fusao(cenario, parceiro, &prob2, &atraso2, &saltos2);
        if (prob2 > prob1 + 0.03 ||
            (fabs(prob2 - prob1) <= 0.03 && atraso2 < atraso1)) {
            fonte = parceiro;
            prob_caminho = prob2;
            atraso_caminho = atraso2;
            saltos = saltos2;
        }
    }
    if (prob_caminho <= 0.0) {
        return 1;
    }
    prazo_restante = acao->prazo - tempo_execucao;
    confianca_fonte = cenario->agentes[fonte].confianca;

    if (estrategia == ESTRATEGIA_GULOSA) {
        redundancia = 0.05;
    } else if (estrategia == ESTRATEGIA_LEILAO) {
        redundancia = 0.18;
    } else {
        redundancia = 0.12 + 1.30 * (1.0 - prob_caminho)
                       + 0.22 * acao->prioridade
                       + 0.12 * (1.0 - confianca_fonte);
        if (estrategia == ESTRATEGIA_CENTRAL) {
            redundancia += 0.10;
        }
        redundancia = limitar(redundancia, 0.18, 1.55);
    }
    transmissoes = (int)ceil((double)acao->simbolos *
                             (1.0 + redundancia));
    if (transmissoes < acao->simbolos) {
        transmissoes = acao->simbolos;
    }
    *pacotes_dados += (double)transmissoes *
                      (double)(saltos > 0 ? saltos : 1);
    prob_caminho *= 0.99 - 0.08 *
                     (double)cenario->agentes[fonte].degradado;
    prob_caminho = limitar(prob_caminho, 0.0, 1.0);
    atraso_obs = 0.0;
    recebidos = 0;
    if (estrategia == ESTRATEGIA_CENTRAL ||
        estrategia == ESTRATEGIA_PROPOSTA) {
        decodificado = transmissao_rlnc(acao->simbolos, transmissoes,
                                        prob_caminho, atraso_caminho,
                                        prazo_restante, g, &atraso_obs,
                                        &recebidos);
    } else {
        decodificado = transmissao_sem_codigo(acao->simbolos, transmissoes,
                                              prob_caminho, atraso_caminho,
                                              prazo_restante, g,
                                              &atraso_obs, &recebidos);
    }
    if (decodificado) {
        *atraso_decodificacao = tempo_execucao + atraso_obs;
        return 2;
    }
    return 1;
}

void executar_estrategia(const Cenario *cenario_base, int estrategia,
                         unsigned long semente, MetricaEpisodio *metrica)
{
    Cenario cenario;
    Gerador g;
    int t;
    int primario;
    int parceiro;
    int estado;
    int execucoes;
    int conclusoes;
    int prazos;
    int selecoes;
    int degradadas;
    int reatribuicoes;
    double conflito;
    double soma_conflitos;
    double pacotes_controle;
    double pacotes_dados;
    double energia_gasta;
    double atraso;
    double soma_atraso;
    double valor_total;
    double valor_entregue;
    int consciente;

    cenario = *cenario_base;
    rng_inicializar(&g, semente);
    memset(metrica, 0, sizeof(MetricaEpisodio));
    execucoes = 0;
    conclusoes = 0;
    prazos = 0;
    selecoes = 0;
    degradadas = 0;
    reatribuicoes = 0;
    soma_conflitos = 0.0;
    pacotes_controle = 0.0;
    pacotes_dados = 0.0;
    energia_gasta = 0.0;
    soma_atraso = 0.0;
    valor_total = 0.0;
    valor_entregue = 0.0;

    for (t = 0; t < cenario.numero_acoes; t++) {
        const Acao *acao;
        acao = &cenario.acoes[t];
        valor_total += acao->valor;
        conflito = 0.0;
        if (estrategia == ESTRATEGIA_CENTRAL) {
            primario = selecionar_central(&cenario, acao);
        } else if (estrategia == ESTRATEGIA_GULOSA) {
            primario = selecionar_guloso(&cenario, acao);
        } else {
            primario = selecionar_leilao(&cenario, acao,
                       estrategia == ESTRATEGIA_ROBUSTA ||
                       estrategia == ESTRATEGIA_PROPOSTA,
                       estrategia == ESTRATEGIA_PROPOSTA,
                       &g, &conflito, &pacotes_controle);
        }
        soma_conflitos += conflito;
        if (primario < 0) {
            reatribuicoes++;
            continue;
        }
        selecoes++;
        if (cenario.agentes[primario].degradado) {
            degradadas++;
        }
        consciente = (estrategia == ESTRATEGIA_CENTRAL ||
                      estrategia == ESTRATEGIA_ROBUSTA ||
                      estrategia == ESTRATEGIA_PROPOSTA);
        parceiro = selecionar_parceiro(&cenario, acao, primario,
                                       consciente);
        if (acao->agentes_minimos > 1 && parceiro < 0) {
            reatribuicoes++;
            continue;
        }
        atraso = 0.0;
        estado = executar_acao(&cenario, acao, estrategia, primario,
                               parceiro, &g, &energia_gasta,
                               &pacotes_dados, &atraso);
        if (estado >= 1) {
            execucoes++;
        } else {
            reatribuicoes++;
        }
        if (estado == 2) {
            conclusoes++;
            prazos++;
            valor_entregue += acao->valor;
            soma_atraso += atraso;
        }
    }

    metrica->execucao_fisica = (double)execucoes /
                               (double)cenario.numero_acoes;
    metrica->conclusao_informacional = (double)conclusoes /
                                       (double)cenario.numero_acoes;
    metrica->valor_entregue = valor_total > 0.0 ?
                              valor_entregue / valor_total : 0.0;
    metrica->cumprimento_prazo = (double)prazos /
                                 (double)cenario.numero_acoes;
    metrica->energia_consumida = energia_gasta /
                                 (double)cenario.numero_agentes;
    metrica->pacotes_transmitidos = (pacotes_controle + pacotes_dados) /
                                    (double)cenario.numero_acoes;
    metrica->conflitos_consenso = soma_conflitos /
                                  (double)cenario.numero_acoes;
    metrica->reatribuicoes = (double)reatribuicoes /
                             (double)cenario.numero_acoes;
    metrica->aceitacoes_degradadas = selecoes > 0 ?
        (double)degradadas / (double)selecoes : 0.0;
    metrica->conectividade = fracao_maior_componente(&cenario);
    metrica->atraso_decodificacao = conclusoes > 0 ?
        soma_atraso / (double)conclusoes : 0.0;
    metrica->continuidade = 0.50 * metrica->valor_entregue
                            + 0.25 * metrica->conclusao_informacional
                            + 0.15 * metrica->conectividade
                            + 0.10 * (1.0 - metrica->reatribuicoes);
}
