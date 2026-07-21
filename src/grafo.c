#include <math.h>
#include "modelo.h"

static void conectar(Cenario *cenario, int i, int j, double d, Gerador *g)
{
    double alcance;
    double perda;
    int atraso;

    alcance = 0.48;
    perda = cenario->perda_global + 0.18 * (d / alcance) * (d / alcance)
            + 0.04 * rng_uniforme(g);
    perda = limitar(perda, 0.01, 0.68);
    atraso = 1 + (int)(7.0 * d / alcance) + rng_inteiro(g, 0, 3);

    cenario->grafo.adj[i][j] = 1;
    cenario->grafo.adj[j][i] = 1;
    cenario->grafo.perda[i][j] = perda;
    cenario->grafo.perda[j][i] = limitar(perda + 0.02 * rng_normal(g), 0.01, 0.72);
    cenario->grafo.atraso[i][j] = atraso;
    cenario->grafo.atraso[j][i] = atraso + rng_inteiro(g, 0, 1);
}

void construir_grafo(Cenario *cenario, Gerador *g)
{
    int i;
    int j;
    int grau;
    int melhor;
    double d;
    double melhor_d;
    double alcance;

    alcance = 0.48;
    for (i = 0; i < cenario->numero_agentes; i++) {
        for (j = 0; j < cenario->numero_agentes; j++) {
            cenario->grafo.adj[i][j] = 0;
            cenario->grafo.perda[i][j] = 1.0;
            cenario->grafo.atraso[i][j] = 0;
        }
    }

    for (i = 0; i < cenario->numero_agentes; i++) {
        for (j = i + 1; j < cenario->numero_agentes; j++) {
            d = distancia(cenario->agentes[i].x, cenario->agentes[i].y,
                          cenario->agentes[j].x, cenario->agentes[j].y);
            if (d <= alcance) {
                conectar(cenario, i, j, d, g);
            }
        }
    }

    for (i = 0; i < cenario->numero_agentes; i++) {
        grau = 0;
        for (j = 0; j < cenario->numero_agentes; j++) {
            grau += cenario->grafo.adj[i][j];
        }
        while (grau < 2) {
            melhor = -1;
            melhor_d = 1.0e30;
            for (j = 0; j < cenario->numero_agentes; j++) {
                if (j == i || cenario->grafo.adj[i][j]) {
                    continue;
                }
                d = distancia(cenario->agentes[i].x,
                              cenario->agentes[i].y,
                              cenario->agentes[j].x,
                              cenario->agentes[j].y);
                if (d < melhor_d) {
                    melhor_d = d;
                    melhor = j;
                }
            }
            if (melhor < 0) {
                break;
            }
            conectar(cenario, i, melhor, melhor_d, g);
            grau++;
        }
    }
}

double centralidade_grau(const Cenario *cenario, int agente)
{
    int j;
    int grau;

    grau = 0;
    for (j = 0; j < cenario->numero_agentes; j++) {
        grau += cenario->grafo.adj[agente][j];
    }
    if (cenario->numero_agentes <= 1) {
        return 0.0;
    }
    return (double)grau / (double)(cenario->numero_agentes - 1);
}

double fracao_maior_componente(const Cenario *cenario)
{
    int visitado[MAX_AGENTES];
    int fila[MAX_AGENTES];
    int i;
    int j;
    int inicio;
    int fim;
    int atual;
    int tamanho;
    int maior;

    for (i = 0; i < cenario->numero_agentes; i++) {
        visitado[i] = 0;
    }
    maior = 0;
    for (i = 0; i < cenario->numero_agentes; i++) {
        if (visitado[i]) {
            continue;
        }
        inicio = 0;
        fim = 0;
        fila[fim++] = i;
        visitado[i] = 1;
        tamanho = 0;
        while (inicio < fim) {
            atual = fila[inicio++];
            tamanho++;
            for (j = 0; j < cenario->numero_agentes; j++) {
                if (cenario->grafo.adj[atual][j] && !visitado[j]) {
                    visitado[j] = 1;
                    fila[fim++] = j;
                }
            }
        }
        if (tamanho > maior) {
            maior = tamanho;
        }
    }
    return (double)maior / (double)cenario->numero_agentes;
}

int melhor_caminho_fusao(const Cenario *cenario, int origem,
                         double *probabilidade, double *atraso_medio,
                         int *saltos)
{
    double custo[MAX_AGENTES];
    double atraso[MAX_AGENTES];
    int hops[MAX_AGENTES];
    int usado[MAX_AGENTES];
    int i;
    int j;
    int atual;
    int melhor_fusao;
    double melhor;
    double novo;
    double s;

    for (i = 0; i < cenario->numero_agentes; i++) {
        custo[i] = 1.0e30;
        atraso[i] = 1.0e30;
        hops[i] = 1000000;
        usado[i] = 0;
    }
    custo[origem] = 0.0;
    atraso[origem] = 0.0;
    hops[origem] = 0;

    for (i = 0; i < cenario->numero_agentes; i++) {
        atual = -1;
        melhor = 1.0e30;
        for (j = 0; j < cenario->numero_agentes; j++) {
            if (!usado[j] && custo[j] < melhor) {
                melhor = custo[j];
                atual = j;
            }
        }
        if (atual < 0) {
            break;
        }
        usado[atual] = 1;
        for (j = 0; j < cenario->numero_agentes; j++) {
            if (!cenario->grafo.adj[atual][j]) {
                continue;
            }
            s = 1.0 - cenario->grafo.perda[atual][j];
            if (s < 1.0e-9) {
                s = 1.0e-9;
            }
            novo = custo[atual] - log(s);
            if (novo < custo[j]) {
                custo[j] = novo;
                atraso[j] = atraso[atual]
                            + (double)cenario->grafo.atraso[atual][j];
                hops[j] = hops[atual] + 1;
            }
        }
    }

    melhor_fusao = -1;
    melhor = 1.0e30;
    for (i = 0; i < cenario->numero_agentes; i++) {
        if ((cenario->agentes[i].capacidades & CAP_FUSAO) == 0U) {
            continue;
        }
        if (i == origem) {
            *probabilidade = 1.0;
            *atraso_medio = 0.5;
            *saltos = 0;
            return i;
        }
        if (custo[i] < melhor) {
            melhor = custo[i];
            melhor_fusao = i;
        }
    }

    if (melhor_fusao < 0 || melhor >= 1.0e29) {
        *probabilidade = 0.0;
        *atraso_medio = 1.0e30;
        *saltos = 0;
        return -1;
    }

    *probabilidade = exp(-custo[melhor_fusao]);
    *atraso_medio = atraso[melhor_fusao];
    *saltos = hops[melhor_fusao];
    return melhor_fusao;
}
