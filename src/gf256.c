#include <string.h>
#include "modelo.h"

unsigned char gf256_multiplicar(unsigned char a, unsigned char b)
{
    unsigned int aa;
    unsigned int bb;
    unsigned int produto;
    int i;

    aa = (unsigned int)a;
    bb = (unsigned int)b;
    produto = 0U;
    for (i = 0; i < 8; i++) {
        if ((bb & 1U) != 0U) {
            produto ^= aa;
        }
        bb >>= 1;
        aa <<= 1;
        if ((aa & 0x100U) != 0U) {
            aa ^= 0x11dU;
        }
    }
    return (unsigned char)(produto & 0xffU);
}

static unsigned char gf256_potencia(unsigned char a, unsigned int expoente)
{
    unsigned char resultado;
    unsigned char base;

    resultado = 1U;
    base = a;
    while (expoente > 0U) {
        if ((expoente & 1U) != 0U) {
            resultado = gf256_multiplicar(resultado, base);
        }
        base = gf256_multiplicar(base, base);
        expoente >>= 1;
    }
    return resultado;
}

unsigned char gf256_inverter(unsigned char a)
{
    if (a == 0U) {
        return 0U;
    }
    return gf256_potencia(a, 254U);
}

int gf256_posto(unsigned char matriz[MAX_SIMBOLOS][MAX_SIMBOLOS],
                int linhas, int colunas)
{
    int posto;
    int coluna;
    int pivo;
    int i;
    int j;
    unsigned char inverso;
    unsigned char fator;
    unsigned char temporario;

    posto = 0;
    for (coluna = 0; coluna < colunas && posto < linhas; coluna++) {
        pivo = -1;
        for (i = posto; i < linhas; i++) {
            if (matriz[i][coluna] != 0U) {
                pivo = i;
                break;
            }
        }
        if (pivo < 0) {
            continue;
        }
        if (pivo != posto) {
            for (j = coluna; j < colunas; j++) {
                temporario = matriz[posto][j];
                matriz[posto][j] = matriz[pivo][j];
                matriz[pivo][j] = temporario;
            }
        }
        inverso = gf256_inverter(matriz[posto][coluna]);
        for (j = coluna; j < colunas; j++) {
            matriz[posto][j] = gf256_multiplicar(matriz[posto][j], inverso);
        }
        for (i = 0; i < linhas; i++) {
            if (i == posto || matriz[i][coluna] == 0U) {
                continue;
            }
            fator = matriz[i][coluna];
            for (j = coluna; j < colunas; j++) {
                matriz[i][j] ^= gf256_multiplicar(fator,
                                                   matriz[posto][j]);
            }
        }
        posto++;
    }
    return posto;
}

static int pacote_chegou(double probabilidade_entrega,
                         double atraso_medio, double prazo_restante,
                         Gerador *g, double *atraso)
{
    double jitter;
    double tempo;

    if (rng_uniforme(g) > probabilidade_entrega) {
        return 0;
    }
    jitter = 0.65 + 0.70 * rng_uniforme(g);
    tempo = atraso_medio * jitter + 0.35 * rng_uniforme(g);
    if (tempo > prazo_restante) {
        return 0;
    }
    *atraso = tempo;
    return 1;
}

int transmissao_rlnc(int simbolos, int transmissoes,
                     double probabilidade_entrega, double atraso_medio,
                     double prazo_restante, Gerador *g,
                     double *atraso_observado, int *recebidos)
{
    unsigned char base[MAX_SIMBOLOS][MAX_SIMBOLOS];
    unsigned char temporaria[MAX_SIMBOLOS][MAX_SIMBOLOS];
    unsigned char linha[MAX_SIMBOLOS];
    int posto;
    int novo_posto;
    int total_recebidos;
    int t;
    int i;
    int j;
    int chegou;
    double atraso;
    double maior_atraso;

    memset(base, 0, sizeof(base));
    posto = 0;
    total_recebidos = 0;
    maior_atraso = 0.0;
    for (t = 0; t < transmissoes; t++) {
        atraso = 0.0;
        chegou = pacote_chegou(probabilidade_entrega, atraso_medio,
                               prazo_restante, g, &atraso);
        if (!chegou) {
            continue;
        }
        total_recebidos++;
        for (j = 0; j < simbolos; j++) {
            if (t < simbolos) {
                linha[j] = (unsigned char)(j == t ? 1 : 0);
            } else {
                linha[j] = (unsigned char)rng_inteiro(g, 0, 255);
            }
        }
        memset(temporaria, 0, sizeof(temporaria));
        for (i = 0; i < posto; i++) {
            for (j = 0; j < simbolos; j++) {
                temporaria[i][j] = base[i][j];
            }
        }
        for (j = 0; j < simbolos; j++) {
            temporaria[posto][j] = linha[j];
        }
        novo_posto = gf256_posto(temporaria, posto + 1, simbolos);
        if (novo_posto > posto) {
            posto = novo_posto;
            for (i = 0; i < posto; i++) {
                for (j = 0; j < simbolos; j++) {
                    base[i][j] = temporaria[i][j];
                }
            }
            if (atraso > maior_atraso) {
                maior_atraso = atraso;
            }
        }
        if (posto >= simbolos) {
            break;
        }
    }
    *recebidos = total_recebidos;
    *atraso_observado = maior_atraso;
    return posto >= simbolos;
}

int transmissao_sem_codigo(int simbolos, int transmissoes,
                           double probabilidade_entrega,
                           double atraso_medio, double prazo_restante,
                           Gerador *g, double *atraso_observado,
                           int *recebidos)
{
    int visto[MAX_SIMBOLOS];
    int t;
    int indice;
    int distintos;
    int chegou;
    double atraso;
    double maior_atraso;

    for (t = 0; t < MAX_SIMBOLOS; t++) {
        visto[t] = 0;
    }
    distintos = 0;
    maior_atraso = 0.0;
    for (t = 0; t < transmissoes; t++) {
        atraso = 0.0;
        chegou = pacote_chegou(probabilidade_entrega, atraso_medio,
                               prazo_restante, g, &atraso);
        if (!chegou) {
            continue;
        }
        indice = t % simbolos;
        if (!visto[indice]) {
            visto[indice] = 1;
            distintos++;
        }
        if (atraso > maior_atraso) {
            maior_atraso = atraso;
        }
    }
    *recebidos = distintos;
    *atraso_observado = maior_atraso;
    return distintos >= simbolos;
}
