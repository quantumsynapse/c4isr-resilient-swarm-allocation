#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "modelo.h"

typedef struct {
    Acumulador perda_global;
    Acumulador degradados;
    Acumulador conectividade;
    Acumulador energia;
    Acumulador saude;
    Acumulador valor_acao;
    Acumulador prazo_acao;
    Acumulador complexidade;
    double perda_minima;
    double perda_maxima;
    double valor_minimo;
    double valor_maximo;
    long cenarios_invalidos;
} ValidacaoSintetica;

static void montar_caminho(char *destino, const char *diretorio,
                           const char *arquivo)
{
    size_t necessario;

    necessario = strlen(diretorio) + strlen(arquivo) + 2U;
    if (necessario >= 512U) {
        destino[0] = '\0';
        return;
    }
    sprintf(destino, "%s/%s", diretorio, arquivo);
}

static void validacao_inicializar(ValidacaoSintetica *v)
{
    acumulador_inicializar(&v->perda_global);
    acumulador_inicializar(&v->degradados);
    acumulador_inicializar(&v->conectividade);
    acumulador_inicializar(&v->energia);
    acumulador_inicializar(&v->saude);
    acumulador_inicializar(&v->valor_acao);
    acumulador_inicializar(&v->prazo_acao);
    acumulador_inicializar(&v->complexidade);
    v->perda_minima = 1.0e30;
    v->perda_maxima = -1.0e30;
    v->valor_minimo = 1.0e30;
    v->valor_maximo = -1.0e30;
    v->cenarios_invalidos = 0L;
}

static void validacao_adicionar(ValidacaoSintetica *v,
                                const Cenario *cenario)
{
    int i;

    acumulador_adicionar(&v->perda_global, cenario->perda_global);
    acumulador_adicionar(&v->degradados,
                         (double)cenario->numero_degradados);
    acumulador_adicionar(&v->conectividade,
                         fracao_maior_componente(cenario));
    if (cenario->perda_global < v->perda_minima) {
        v->perda_minima = cenario->perda_global;
    }
    if (cenario->perda_global > v->perda_maxima) {
        v->perda_maxima = cenario->perda_global;
    }
    for (i = 0; i < cenario->numero_agentes; i++) {
        acumulador_adicionar(&v->energia, cenario->agentes[i].energia);
        acumulador_adicionar(&v->saude, cenario->agentes[i].saude);
    }
    for (i = 0; i < cenario->numero_acoes; i++) {
        acumulador_adicionar(&v->valor_acao, cenario->acoes[i].valor);
        acumulador_adicionar(&v->prazo_acao, cenario->acoes[i].prazo);
        acumulador_adicionar(&v->complexidade,
                             cenario->acoes[i].complexidade);
        if (cenario->acoes[i].valor < v->valor_minimo) {
            v->valor_minimo = cenario->acoes[i].valor;
        }
        if (cenario->acoes[i].valor > v->valor_maximo) {
            v->valor_maximo = cenario->acoes[i].valor;
        }
    }
    if (validar_cenario(cenario, NULL) > 0.0) {
        v->cenarios_invalidos++;
    }
}

static void escrever_validacao(const char *diretorio,
                               const ValidacaoSintetica *v,
                               int episodios, unsigned long semente)
{
    char caminho[512];
    FILE *f;
    ResumoEstatistico r;

    montar_caminho(caminho, diretorio, "validacao_dados.txt");
    f = fopen(caminho, "w");
    if (f == NULL) {
        return;
    }
    fprintf(f, "VALIDACAO DOS DADOS SINTETICOS\n");
    fprintf(f, "================================\n\n");
    fprintf(f, "semente=%lu\n", semente);
    fprintf(f, "cenarios=%d\n", episodios);
    fprintf(f, "registros_agente=%ld\n", v->energia.n);
    fprintf(f, "registros_acao=%ld\n", v->valor_acao.n);
    fprintf(f, "cenarios_invalidos=%ld\n", v->cenarios_invalidos);

    r = acumulador_resumir(&v->perda_global);
    fprintf(f, "perda_global_media=%.8f\n", r.media);
    fprintf(f, "perda_global_minima=%.8f\n", v->perda_minima);
    fprintf(f, "perda_global_maxima=%.8f\n", v->perda_maxima);
    r = acumulador_resumir(&v->degradados);
    fprintf(f, "agentes_degradados_media=%.8f\n", r.media);
    r = acumulador_resumir(&v->conectividade);
    fprintf(f, "maior_componente_media=%.8f\n", r.media);
    r = acumulador_resumir(&v->energia);
    fprintf(f, "energia_inicial_media=%.8f\n", r.media);
    r = acumulador_resumir(&v->saude);
    fprintf(f, "saude_inicial_media=%.8f\n", r.media);
    r = acumulador_resumir(&v->valor_acao);
    fprintf(f, "valor_acao_media=%.8f\n", r.media);
    fprintf(f, "valor_acao_minimo=%.8f\n", v->valor_minimo);
    fprintf(f, "valor_acao_maximo=%.8f\n", v->valor_maximo);
    r = acumulador_resumir(&v->prazo_acao);
    fprintf(f, "prazo_acao_media=%.8f\n", r.media);
    r = acumulador_resumir(&v->complexidade);
    fprintf(f, "complexidade_media=%.8f\n", r.media);
    fprintf(f, "status=%s\n",
            v->cenarios_invalidos == 0L ? "aprovado" : "reprovado");
    fclose(f);
}

static void escrever_cabecalho_resumo(FILE *f)
{
    fprintf(f, "estrategia,metrica,media,desvio,erro_padrao,ic95_inferior,ic95_superior,n\n");
}

static void escrever_linha_resumo(FILE *f, const char *estrategia,
                                  const char *metrica,
                                  const Acumulador *a)
{
    ResumoEstatistico r;

    r = acumulador_resumir(a);
    fprintf(f, "%s,%s,%.10f,%.10f,%.10f,%.10f,%.10f,%ld\n",
            estrategia, metrica, r.media, r.desvio, r.erro_padrao,
            r.limite_inferior, r.limite_superior, a->n);
}

static void escrever_resumo_csv(const char *diretorio,
                                EstatisticaEstrategia estatisticas[])
{
    char caminho[512];
    FILE *f;
    int s;
    const char *nome;

    montar_caminho(caminho, diretorio, "resumo_monte_carlo.csv");
    f = fopen(caminho, "w");
    if (f == NULL) {
        return;
    }
    escrever_cabecalho_resumo(f);
    for (s = 0; s < NUM_ESTRATEGIAS; s++) {
        nome = nome_estrategia(s);
        escrever_linha_resumo(f, nome, "execucao_fisica",
                              &estatisticas[s].execucao_fisica);
        escrever_linha_resumo(f, nome, "conclusao_informacional",
                              &estatisticas[s].conclusao_informacional);
        escrever_linha_resumo(f, nome, "valor_entregue",
                              &estatisticas[s].valor_entregue);
        escrever_linha_resumo(f, nome, "cumprimento_prazo",
                              &estatisticas[s].cumprimento_prazo);
        escrever_linha_resumo(f, nome, "energia_consumida",
                              &estatisticas[s].energia_consumida);
        escrever_linha_resumo(f, nome, "pacotes_transmitidos",
                              &estatisticas[s].pacotes_transmitidos);
        escrever_linha_resumo(f, nome, "conflitos_consenso",
                              &estatisticas[s].conflitos_consenso);
        escrever_linha_resumo(f, nome, "reatribuicoes",
                              &estatisticas[s].reatribuicoes);
        escrever_linha_resumo(f, nome, "aceitacoes_degradadas",
                              &estatisticas[s].aceitacoes_degradadas);
        escrever_linha_resumo(f, nome, "conectividade",
                              &estatisticas[s].conectividade);
        escrever_linha_resumo(f, nome, "atraso_decodificacao",
                              &estatisticas[s].atraso_decodificacao);
        escrever_linha_resumo(f, nome, "continuidade",
                              &estatisticas[s].continuidade);
    }
    fclose(f);
}

static void escrever_hipotese(const char *diretorio,
                              const Acumulador *d_conclusao_robusta,
                              const Acumulador *d_valor_robusta,
                              const Acumulador *d_continuidade_robusta,
                              const Acumulador *d_pacotes_robusta,
                              const Acumulador *d_conclusao_leilao,
                              const Acumulador *d_valor_leilao)
{
    char caminho[512];
    FILE *f;
    ResumoEstatistico r;
    double t;

    montar_caminho(caminho, diretorio, "teste_hipotese.csv");
    f = fopen(caminho, "w");
    if (f == NULL) {
        return;
    }
    fprintf(f, "comparacao,metrica,diferenca_media,erro_padrao,ic95_inferior,ic95_superior,t_aproximado,n\n");

#define ESCREVER_DIF(COMP,MET,ACC) \
    r = acumulador_resumir((ACC)); \
    t = r.erro_padrao > 0.0 ? r.media / r.erro_padrao : 0.0; \
    fprintf(f, "%s,%s,%.10f,%.10f,%.10f,%.10f,%.10f,%ld\n", \
            (COMP), (MET), r.media, r.erro_padrao, r.limite_inferior, \
            r.limite_superior, t, (ACC)->n)

    ESCREVER_DIF("proposta_menos_robusta", "conclusao_informacional",
                 d_conclusao_robusta);
    ESCREVER_DIF("proposta_menos_robusta", "valor_entregue",
                 d_valor_robusta);
    ESCREVER_DIF("proposta_menos_robusta", "continuidade",
                 d_continuidade_robusta);
    ESCREVER_DIF("proposta_menos_robusta", "pacotes_transmitidos",
                 d_pacotes_robusta);
    ESCREVER_DIF("proposta_menos_leilao", "conclusao_informacional",
                 d_conclusao_leilao);
    ESCREVER_DIF("proposta_menos_leilao", "valor_entregue",
                 d_valor_leilao);
#undef ESCREVER_DIF
    fclose(f);
}

static void escrever_convergencia_linha(FILE *f, int episodio,
                                        EstatisticaEstrategia estatisticas[])
{
    ResumoEstatistico rp;
    ResumoEstatistico rr;
    ResumoEstatistico vp;
    ResumoEstatistico vr;

    rp = acumulador_resumir(
        &estatisticas[ESTRATEGIA_PROPOSTA].conclusao_informacional);
    rr = acumulador_resumir(
        &estatisticas[ESTRATEGIA_ROBUSTA].conclusao_informacional);
    vp = acumulador_resumir(
        &estatisticas[ESTRATEGIA_PROPOSTA].valor_entregue);
    vr = acumulador_resumir(
        &estatisticas[ESTRATEGIA_ROBUSTA].valor_entregue);
    fprintf(f, "%d,%.10f,%.10f,%.10f,%.10f\n", episodio,
            rp.media, rr.media, vp.media, vr.media);
}

static void executar_sensibilidade_perda(const ConfiguracaoExecucao *cfg)
{
    static const double niveis[] = {0.05, 0.15, 0.25, 0.35};
    char caminho[512];
    FILE *f;
    int l;
    int e;
    int s;
    int episodios;
    Gerador mestre;
    Cenario cenario;
    MetricaEpisodio m;
    EstatisticaEstrategia est[NUM_ESTRATEGIAS];
    unsigned long base;

    episodios = cfg->episodios / 5;
    if (episodios < 250) {
        episodios = 250;
    }
    if (episodios > 800) {
        episodios = 800;
    }
    montar_caminho(caminho, cfg->diretorio_saida,
                   "sensibilidade_perda.csv");
    f = fopen(caminho, "w");
    if (f == NULL) {
        return;
    }
    fprintf(f, "perda_global,estrategia,conclusao_informacional,valor_entregue,continuidade,pacotes_transmitidos,episodios\n");
    for (l = 0; l < 4; l++) {
        for (s = 0; s < NUM_ESTRATEGIAS; s++) {
            estatistica_inicializar(&est[s]);
        }
        rng_inicializar(&mestre, cfg->semente + 10000UL +
                        (unsigned long)l * 991UL);
        for (e = 0; e < episodios; e++) {
            base = rng_proximo(&mestre);
            gerar_cenario(&cenario, &mestre, niveis[l], -1, -1.0);
            for (s = 0; s < NUM_ESTRATEGIAS; s++) {
                executar_estrategia(&cenario, s,
                    base ^ (2654435761UL * (unsigned long)(s + 1)), &m);
                estatistica_adicionar(&est[s], &m);
            }
        }
        for (s = 0; s < NUM_ESTRATEGIAS; s++) {
            fprintf(f, "%.4f,%s,%.10f,%.10f,%.10f,%.10f,%d\n",
                    niveis[l], nome_estrategia(s),
                    acumulador_resumir(&est[s].conclusao_informacional).media,
                    acumulador_resumir(&est[s].valor_entregue).media,
                    acumulador_resumir(&est[s].continuidade).media,
                    acumulador_resumir(&est[s].pacotes_transmitidos).media,
                    episodios);
        }
    }
    fclose(f);
}

static void executar_sensibilidade_perturbacao(
    const ConfiguracaoExecucao *cfg)
{
    static const double niveis[] = {0.00, 0.15, 0.30, 0.45};
    char caminho[512];
    FILE *f;
    int l;
    int e;
    int s;
    int episodios;
    Gerador mestre;
    Cenario cenario;
    MetricaEpisodio m;
    EstatisticaEstrategia est[NUM_ESTRATEGIAS];
    unsigned long base;

    episodios = cfg->episodios / 5;
    if (episodios < 250) {
        episodios = 250;
    }
    if (episodios > 800) {
        episodios = 800;
    }
    montar_caminho(caminho, cfg->diretorio_saida,
                   "sensibilidade_perturbacao.csv");
    f = fopen(caminho, "w");
    if (f == NULL) {
        return;
    }
    fprintf(f, "amplitude_perturbacao,estrategia,conclusao_informacional,valor_entregue,aceitacoes_degradadas,conflitos_consenso,episodios\n");
    for (l = 0; l < 4; l++) {
        for (s = 0; s < NUM_ESTRATEGIAS; s++) {
            estatistica_inicializar(&est[s]);
        }
        rng_inicializar(&mestre, cfg->semente + 20000UL +
                        (unsigned long)l * 1237UL);
        for (e = 0; e < episodios; e++) {
            base = rng_proximo(&mestre);
            gerar_cenario(&cenario, &mestre, -1.0, 3, niveis[l]);
            for (s = 0; s < NUM_ESTRATEGIAS; s++) {
                executar_estrategia(&cenario, s,
                    base ^ (2654435761UL * (unsigned long)(s + 1)), &m);
                estatistica_adicionar(&est[s], &m);
            }
        }
        for (s = 0; s < NUM_ESTRATEGIAS; s++) {
            fprintf(f, "%.4f,%s,%.10f,%.10f,%.10f,%.10f,%d\n",
                    niveis[l], nome_estrategia(s),
                    acumulador_resumir(&est[s].conclusao_informacional).media,
                    acumulador_resumir(&est[s].valor_entregue).media,
                    acumulador_resumir(&est[s].aceitacoes_degradadas).media,
                    acumulador_resumir(&est[s].conflitos_consenso).media,
                    episodios);
        }
    }
    fclose(f);
}

static void escrever_resumo_texto(const ConfiguracaoExecucao *cfg,
                                  EstatisticaEstrategia estatisticas[],
                                  const Acumulador *d_conclusao,
                                  const Acumulador *d_valor,
                                  const Acumulador *d_continuidade)
{
    char caminho[512];
    FILE *f;
    int s;
    ResumoEstatistico r;
    ResumoEstatistico dc;
    ResumoEstatistico dv;
    ResumoEstatistico di;

    montar_caminho(caminho, cfg->diretorio_saida,
                   "resumo_referencia.txt");
    f = fopen(caminho, "w");
    if (f == NULL) {
        return;
    }
    fprintf(f, "SIMULACAO C4ISR SINTETICA\n");
    fprintf(f, "==========================\n\n");
    fprintf(f, "episodios_por_estrategia=%d\n", cfg->episodios);
    fprintf(f, "estrategias=%d\n", NUM_ESTRATEGIAS);
    fprintf(f, "episodios_estrategia_total=%d\n",
            cfg->episodios * NUM_ESTRATEGIAS);
    fprintf(f, "agentes_por_cenario=%d\n", MAX_AGENTES);
    fprintf(f, "acoes_por_cenario=%d\n", MAX_ACOES);
    fprintf(f, "semente=%lu\n\n", cfg->semente);

    for (s = 0; s < NUM_ESTRATEGIAS; s++) {
        fprintf(f, "[%s]\n", nome_estrategia(s));
        r = acumulador_resumir(&estatisticas[s].conclusao_informacional);
        fprintf(f, "conclusao_informacional=%.6f [%.6f, %.6f]\n",
                r.media, r.limite_inferior, r.limite_superior);
        r = acumulador_resumir(&estatisticas[s].valor_entregue);
        fprintf(f, "valor_entregue=%.6f [%.6f, %.6f]\n",
                r.media, r.limite_inferior, r.limite_superior);
        r = acumulador_resumir(&estatisticas[s].continuidade);
        fprintf(f, "continuidade=%.6f [%.6f, %.6f]\n",
                r.media, r.limite_inferior, r.limite_superior);
        r = acumulador_resumir(&estatisticas[s].pacotes_transmitidos);
        fprintf(f, "pacotes_por_acao=%.6f\n", r.media);
        r = acumulador_resumir(&estatisticas[s].aceitacoes_degradadas);
        fprintf(f, "aceitacoes_degradadas=%.6f\n", r.media);
        r = acumulador_resumir(&estatisticas[s].reatribuicoes);
        fprintf(f, "reatribuicoes=%.6f\n\n", r.media);
    }

    dc = acumulador_resumir(d_conclusao);
    dv = acumulador_resumir(d_valor);
    di = acumulador_resumir(d_continuidade);
    fprintf(f, "[HIPOTESE PRINCIPAL]\n");
    fprintf(f, "proposta_menos_robusta_conclusao=%.6f [%.6f, %.6f]\n",
            dc.media, dc.limite_inferior, dc.limite_superior);
    fprintf(f, "proposta_menos_robusta_valor=%.6f [%.6f, %.6f]\n",
            dv.media, dv.limite_inferior, dv.limite_superior);
    fprintf(f, "proposta_menos_robusta_continuidade=%.6f [%.6f, %.6f]\n",
            di.media, di.limite_inferior, di.limite_superior);
    fprintf(f, "hipotese_suportada=%s\n",
            dc.limite_inferior > 0.0 &&
            dv.limite_inferior > 0.0 &&
            di.limite_inferior > 0.0 ? "sim" : "nao");
    fclose(f);
}

int executar_experimento(const ConfiguracaoExecucao *cfg)
{
    EstatisticaEstrategia estatisticas[NUM_ESTRATEGIAS];
    Acumulador d_conclusao_robusta;
    Acumulador d_valor_robusta;
    Acumulador d_continuidade_robusta;
    Acumulador d_pacotes_robusta;
    Acumulador d_conclusao_leilao;
    Acumulador d_valor_leilao;
    ValidacaoSintetica validacao;
    Gerador mestre;
    Cenario cenario;
    MetricaEpisodio metricas[NUM_ESTRATEGIAS];
    unsigned long base;
    int e;
    int s;
    int checkpoints[4];
    int proximo_checkpoint;
    char caminho[512];
    FILE *fconv;

    if (cfg->episodios <= 0 || cfg->diretorio_saida == NULL) {
        return 1;
    }
    for (s = 0; s < NUM_ESTRATEGIAS; s++) {
        estatistica_inicializar(&estatisticas[s]);
    }
    acumulador_inicializar(&d_conclusao_robusta);
    acumulador_inicializar(&d_valor_robusta);
    acumulador_inicializar(&d_continuidade_robusta);
    acumulador_inicializar(&d_pacotes_robusta);
    acumulador_inicializar(&d_conclusao_leilao);
    acumulador_inicializar(&d_valor_leilao);
    validacao_inicializar(&validacao);
    rng_inicializar(&mestre, cfg->semente);

    checkpoints[0] = cfg->episodios / 10;
    checkpoints[1] = cfg->episodios / 4;
    checkpoints[2] = cfg->episodios / 2;
    checkpoints[3] = cfg->episodios;
    if (checkpoints[0] < 1) checkpoints[0] = 1;
    if (checkpoints[1] <= checkpoints[0]) checkpoints[1] = checkpoints[0] + 1;
    if (checkpoints[2] <= checkpoints[1]) checkpoints[2] = checkpoints[1] + 1;
    if (checkpoints[3] <= checkpoints[2]) checkpoints[3] = checkpoints[2] + 1;

    montar_caminho(caminho, cfg->diretorio_saida, "convergencia.csv");
    fconv = fopen(caminho, "w");
    if (fconv != NULL) {
        fprintf(fconv, "episodios,proposta_conclusao,robusta_conclusao,proposta_valor,robusta_valor\n");
    }
    proximo_checkpoint = 0;

    for (e = 0; e < cfg->episodios; e++) {
        base = rng_proximo(&mestre);
        gerar_cenario(&cenario, &mestre, cfg->perda_forcada,
                      cfg->degradados_forcados,
                      cfg->perturbacao_forcada);
        validacao_adicionar(&validacao, &cenario);
        for (s = 0; s < NUM_ESTRATEGIAS; s++) {
            executar_estrategia(&cenario, s,
                base ^ (2654435761UL * (unsigned long)(s + 1)),
                &metricas[s]);
            estatistica_adicionar(&estatisticas[s], &metricas[s]);
        }
        acumulador_adicionar(&d_conclusao_robusta,
            metricas[ESTRATEGIA_PROPOSTA].conclusao_informacional -
            metricas[ESTRATEGIA_ROBUSTA].conclusao_informacional);
        acumulador_adicionar(&d_valor_robusta,
            metricas[ESTRATEGIA_PROPOSTA].valor_entregue -
            metricas[ESTRATEGIA_ROBUSTA].valor_entregue);
        acumulador_adicionar(&d_continuidade_robusta,
            metricas[ESTRATEGIA_PROPOSTA].continuidade -
            metricas[ESTRATEGIA_ROBUSTA].continuidade);
        acumulador_adicionar(&d_pacotes_robusta,
            metricas[ESTRATEGIA_PROPOSTA].pacotes_transmitidos -
            metricas[ESTRATEGIA_ROBUSTA].pacotes_transmitidos);
        acumulador_adicionar(&d_conclusao_leilao,
            metricas[ESTRATEGIA_PROPOSTA].conclusao_informacional -
            metricas[ESTRATEGIA_LEILAO].conclusao_informacional);
        acumulador_adicionar(&d_valor_leilao,
            metricas[ESTRATEGIA_PROPOSTA].valor_entregue -
            metricas[ESTRATEGIA_LEILAO].valor_entregue);

        if (fconv != NULL && proximo_checkpoint < 4 &&
            e + 1 >= checkpoints[proximo_checkpoint]) {
            escrever_convergencia_linha(fconv, e + 1, estatisticas);
            proximo_checkpoint++;
        }
    }
    if (fconv != NULL) {
        fclose(fconv);
    }

    escrever_validacao(cfg->diretorio_saida, &validacao, cfg->episodios,
                       cfg->semente);
    escrever_resumo_csv(cfg->diretorio_saida, estatisticas);
    escrever_hipotese(cfg->diretorio_saida,
                      &d_conclusao_robusta, &d_valor_robusta,
                      &d_continuidade_robusta, &d_pacotes_robusta,
                      &d_conclusao_leilao, &d_valor_leilao);
    escrever_resumo_texto(cfg, estatisticas, &d_conclusao_robusta,
                          &d_valor_robusta, &d_continuidade_robusta);

    if (cfg->executar_sensibilidade) {
        executar_sensibilidade_perda(cfg);
        executar_sensibilidade_perturbacao(cfg);
    }
    return validacao.cenarios_invalidos == 0L ? 0 : 2;
}
