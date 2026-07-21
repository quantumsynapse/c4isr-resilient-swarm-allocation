#include <math.h>
#include "modelo.h"

double limitar(double valor, double minimo, double maximo)
{
    if (valor < minimo) {
        return minimo;
    }
    if (valor > maximo) {
        return maximo;
    }
    return valor;
}

double distancia(double x1, double y1, double x2, double y2)
{
    double dx;
    double dy;

    dx = x1 - x2;
    dy = y1 - y2;
    return sqrt(dx * dx + dy * dy);
}

int contar_bits(unsigned int valor)
{
    int total;

    total = 0;
    while (valor != 0U) {
        total += (int)(valor & 1U);
        valor >>= 1;
    }
    return total;
}

int valor_finito(double valor)
{
    if (valor != valor) {
        return 0;
    }
    if (valor > 1.0e300 || valor < -1.0e300) {
        return 0;
    }
    return 1;
}

const char *nome_estrategia(int estrategia)
{
    switch (estrategia) {
        case ESTRATEGIA_CENTRAL:
            return "central_global_rlnc";
        case ESTRATEGIA_GULOSA:
            return "gulosa_distribuida";
        case ESTRATEGIA_LEILAO:
            return "leilao_distribuido";
        case ESTRATEGIA_ROBUSTA:
            return "leilao_resiliente_sem_codigo";
        case ESTRATEGIA_PROPOSTA:
            return "carma_rlnc_adaptativa";
        default:
            return "desconhecida";
    }
}
