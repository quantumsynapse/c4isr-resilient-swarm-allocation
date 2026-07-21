#include <stdio.h>
#include <string.h>
#include "modelo.h"

int main(void)
{
    int a;
    int i;
    int j;
    unsigned char inv;
    unsigned char matriz[MAX_SIMBOLOS][MAX_SIMBOLOS];
    Gerador g;
    double atraso;
    int recebidos;

    for (a = 1; a < 256; a++) {
        inv = gf256_inverter((unsigned char)a);
        if (gf256_multiplicar((unsigned char)a, inv) != 1U) {
            fprintf(stderr, "falha_inverso=%d\n", a);
            return 1;
        }
    }

    memset(matriz, 0, sizeof(matriz));
    for (i = 0; i < MAX_SIMBOLOS; i++) {
        for (j = 0; j < MAX_SIMBOLOS; j++) {
            matriz[i][j] = (unsigned char)(i == j ? 1 : 0);
        }
    }
    if (gf256_posto(matriz, MAX_SIMBOLOS, MAX_SIMBOLOS) != MAX_SIMBOLOS) {
        fprintf(stderr, "falha_posto_identidade\n");
        return 1;
    }

    rng_inicializar(&g, 123UL);
    if (!transmissao_rlnc(8, 8, 1.0, 1.0, 10.0, &g,
                          &atraso, &recebidos)) {
        fprintf(stderr, "falha_rlnc_sem_perda\n");
        return 1;
    }
    rng_inicializar(&g, 123UL);
    if (transmissao_rlnc(8, 16, 0.0, 1.0, 10.0, &g,
                         &atraso, &recebidos)) {
        fprintf(stderr, "falha_rlnc_perda_total\n");
        return 1;
    }
    printf("teste_gf256=aprovado\n");
    return 0;
}
