#include <math.h>
#include <string.h>
#include "modelo.h"

static unsigned int capacidade_aleatoria(Gerador *g, int excluir_fusao)
{
    int limite;
    int indice;

    limite = excluir_fusao ? 5 : 6;
    indice = rng_inteiro(g, 0, limite - 1);
    return 1U << indice;
}

static void gerar_agentes(Cenario *cenario, Gerador *g,
                          int degradados_forcados,
                          double perturbacao_forcada)
{
    int i;
    int j;
    int indice;
    int degradados;
    unsigned int mascara;
    double u;

    degradados = degradados_forcados;
    if (degradados < 0) {
        degradados = rng_poisson(g, 1.4, 4);
    }
    if (degradados > 4) {
        degradados = 4;
    }
    cenario->numero_degradados = degradados;

    for (i = 0; i < cenario->numero_agentes; i++) {
        cenario->agentes[i].id = i;
        cenario->agentes[i].x = rng_uniforme(g);
        cenario->agentes[i].y = rng_uniforme(g);
        cenario->agentes[i].energia = 0.68 + 0.32 *
            (rng_uniforme(g) + rng_uniforme(g)) / 2.0;
        cenario->agentes[i].saude = 0.78 + 0.22 *
            (rng_uniforme(g) + rng_uniforme(g)) / 2.0;
        cenario->agentes[i].confianca = 1.0;
        cenario->agentes[i].carga = 0.0;
        cenario->agentes[i].perda_base = 0.02 + 0.10 * rng_uniforme(g);
        cenario->agentes[i].vies_objetivo = -0.08 + 0.16 * rng_uniforme(g);
        mascara = 1U << (i % MAX_CAPACIDADES);
        mascara |= 1U << ((i + 2) % MAX_CAPACIDADES);
        if (rng_uniforme(g) < 0.55) {
            mascara |= capacidade_aleatoria(g, 0);
        }
        if ((i % 4) == 0) {
            mascara |= CAP_RELE;
        }
        if (i < 3) {
            mascara |= CAP_FUSAO;
        }
        cenario->agentes[i].capacidades = mascara;
        cenario->agentes[i].degradado = 0;
        cenario->agentes[i].perturbacao_persistente = 0.0;
    }

    for (j = 0; j < degradados; j++) {
        do {
            indice = rng_inteiro(g, 0, cenario->numero_agentes - 1);
        } while (cenario->agentes[indice].degradado);
        cenario->agentes[indice].degradado = 1;
        u = 0.20 + 0.30 * rng_uniforme(g);
        if (rng_uniforme(g) < 0.5) {
            u = -u;
        }
        if (perturbacao_forcada >= 0.0) {
            u = (u < 0.0 ? -1.0 : 1.0) * perturbacao_forcada;
        }
        cenario->agentes[indice].perturbacao_persistente = u;
        cenario->agentes[indice].saude = limitar(
            cenario->agentes[indice].saude - 0.08 * rng_uniforme(g),
            0.55, 1.0);
    }
}

static void gerar_acoes(Cenario *cenario, Gerador *g)
{
    int i;
    unsigned int r1;
    unsigned int r2;
    double z;
    double valor;

    for (i = 0; i < cenario->numero_acoes; i++) {
        cenario->acoes[i].id = i;
        cenario->acoes[i].x = rng_uniforme(g);
        cenario->acoes[i].y = rng_uniforme(g);
        cenario->acoes[i].prioridade = 0.45 + 0.55 *
            (rng_uniforme(g) + rng_uniforme(g)) / 2.0;
        z = rng_normal(g);
        valor = exp(3.35 + 0.55 * z);
        cenario->acoes[i].valor = limitar(valor, 12.0, 120.0);
        cenario->acoes[i].prazo = 22.0 + 34.0 *
            (rng_uniforme(g) + rng_uniforme(g)) / 2.0;
        cenario->acoes[i].complexidade = 0.20 + 0.80 *
            (rng_uniforme(g) + rng_uniforme(g)) / 2.0;
        r1 = capacidade_aleatoria(g, 1);
        r2 = 0U;
        if (rng_uniforme(g) < 0.38) {
            do {
                r2 = capacidade_aleatoria(g, 1);
            } while (r2 == r1);
        }
        cenario->acoes[i].capacidades_requeridas = r1 | r2;
        cenario->acoes[i].agentes_minimos =
            (rng_uniforme(g) < 0.24) ? 2 : 1;
        cenario->acoes[i].simbolos = rng_inteiro(g, 4, MAX_SIMBOLOS);
    }
}

void gerar_cenario(Cenario *cenario, Gerador *g, double perda_forcada,
                   int degradados_forcados, double perturbacao_forcada)
{
    double u;

    memset(cenario, 0, sizeof(Cenario));
    cenario->numero_agentes = MAX_AGENTES;
    cenario->numero_acoes = MAX_ACOES;

    if (perda_forcada >= 0.0) {
        cenario->perda_global = limitar(perda_forcada, 0.0, 0.55);
    } else {
        u = rng_uniforme(g);
        cenario->perda_global = 0.025 + 0.30 * pow(u, 1.65);
    }

    cenario->escala_perturbacao = perturbacao_forcada;
    gerar_agentes(cenario, g, degradados_forcados, perturbacao_forcada);
    construir_grafo(cenario, g);
    gerar_acoes(cenario, g);
}

double validar_cenario(const Cenario *cenario, FILE *saida)
{
    int i;
    int erros;
    int fusao;
    int cobertura[MAX_CAPACIDADES];
    int c;
    unsigned int mascara_valida;

    erros = 0;
    fusao = 0;
    mascara_valida = (1U << MAX_CAPACIDADES) - 1U;
    for (c = 0; c < MAX_CAPACIDADES; c++) {
        cobertura[c] = 0;
    }

    if (cenario->numero_agentes != MAX_AGENTES ||
        cenario->numero_acoes != MAX_ACOES) {
        erros++;
    }
    if (!valor_finito(cenario->perda_global) ||
        cenario->perda_global < 0.0 || cenario->perda_global > 0.55) {
        erros++;
    }

    for (i = 0; i < cenario->numero_agentes; i++) {
        const Agente *a;
        a = &cenario->agentes[i];
        if (!valor_finito(a->x) || !valor_finito(a->y) ||
            a->x < 0.0 || a->x > 1.0 || a->y < 0.0 || a->y > 1.0) {
            erros++;
        }
        if (!valor_finito(a->energia) || a->energia < 0.0 ||
            a->energia > 1.0 || !valor_finito(a->saude) ||
            a->saude < 0.0 || a->saude > 1.0) {
            erros++;
        }
        if (a->capacidades == 0U ||
            (a->capacidades & ~mascara_valida) != 0U) {
            erros++;
        }
        if ((a->capacidades & CAP_FUSAO) != 0U) {
            fusao++;
        }
        for (c = 0; c < MAX_CAPACIDADES; c++) {
            if ((a->capacidades & (1U << c)) != 0U) {
                cobertura[c]++;
            }
        }
    }
    if (fusao < 3) {
        erros++;
    }
    for (c = 0; c < MAX_CAPACIDADES; c++) {
        if (cobertura[c] == 0) {
            erros++;
        }
    }

    for (i = 0; i < cenario->numero_acoes; i++) {
        const Acao *t;
        t = &cenario->acoes[i];
        if (!valor_finito(t->valor) || t->valor < 12.0 ||
            t->valor > 120.0 || !valor_finito(t->prazo) ||
            t->prazo < 20.0 || t->prazo > 58.0) {
            erros++;
        }
        if (t->capacidades_requeridas == 0U ||
            (t->capacidades_requeridas & ~31U) != 0U) {
            erros++;
        }
        if (t->agentes_minimos < 1 || t->agentes_minimos > 2 ||
            t->simbolos < 4 || t->simbolos > MAX_SIMBOLOS) {
            erros++;
        }
    }

    if (saida != NULL) {
        fprintf(saida, "erros=%d\n", erros);
        fprintf(saida, "agentes=%d\n", cenario->numero_agentes);
        fprintf(saida, "acoes=%d\n", cenario->numero_acoes);
        fprintf(saida, "perda_global=%.6f\n", cenario->perda_global);
        fprintf(saida, "degradados=%d\n", cenario->numero_degradados);
        fprintf(saida, "fusao=%d\n", fusao);
        fprintf(saida, "maior_componente=%.6f\n",
                fracao_maior_componente(cenario));
    }
    return (double)erros;
}
