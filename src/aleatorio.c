#include <math.h>
#include "modelo.h"

void rng_inicializar(Gerador *g, unsigned long semente)
{
    if (semente == 0UL) {
        semente = 1UL;
    }
    g->estado = semente & 0xffffffffUL;
    g->possui_gaussiana = 0;
    g->gaussiana_reserva = 0.0;
}

unsigned long rng_proximo(Gerador *g)
{
    g->estado = (1664525UL * g->estado + 1013904223UL) & 0xffffffffUL;
    return g->estado;
}

double rng_uniforme(Gerador *g)
{
    return ((double)rng_proximo(g) + 0.5) / 4294967296.0;
}

int rng_inteiro(Gerador *g, int minimo, int maximo)
{
    double u;
    int largura;

    if (maximo <= minimo) {
        return minimo;
    }
    largura = maximo - minimo + 1;
    u = rng_uniforme(g);
    return minimo + (int)(u * (double)largura);
}

double rng_normal(Gerador *g)
{
    double u1;
    double u2;
    double raio;
    double angulo;

    if (g->possui_gaussiana) {
        g->possui_gaussiana = 0;
        return g->gaussiana_reserva;
    }

    u1 = rng_uniforme(g);
    u2 = rng_uniforme(g);
    if (u1 < 1.0e-12) {
        u1 = 1.0e-12;
    }
    raio = sqrt(-2.0 * log(u1));
    angulo = 6.28318530717958647692 * u2;
    g->gaussiana_reserva = raio * sin(angulo);
    g->possui_gaussiana = 1;
    return raio * cos(angulo);
}

int rng_poisson(Gerador *g, double lambda, int limite)
{
    double l;
    double p;
    int k;

    if (lambda <= 0.0) {
        return 0;
    }
    l = exp(-lambda);
    p = 1.0;
    k = 0;
    do {
        k++;
        p *= rng_uniforme(g);
    } while (p > l && k <= limite + 1);

    k--;
    if (k > limite) {
        k = limite;
    }
    return k;
}
