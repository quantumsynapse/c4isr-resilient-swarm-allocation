#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "modelo.h"

static void uso(const char *programa)
{
    fprintf(stderr,
        "Uso: %s [--episodios N] [--semente N] [--saida DIR] "
        "[--sem-sensibilidade] [--perda P] [--degradados N] "
        "[--perturbacao A]\n", programa);
}

int main(int argc, char **argv)
{
    ConfiguracaoExecucao cfg;
    int i;

    cfg.episodios = 3000;
    cfg.semente = 1995UL;
    cfg.perda_forcada = -1.0;
    cfg.degradados_forcados = -1;
    cfg.perturbacao_forcada = -1.0;
    cfg.executar_sensibilidade = 1;
    cfg.diretorio_saida = "resultados";

    i = 1;
    while (i < argc) {
        if (strcmp(argv[i], "--episodios") == 0 && i + 1 < argc) {
            cfg.episodios = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--semente") == 0 && i + 1 < argc) {
            cfg.semente = strtoul(argv[++i], NULL, 10);
        } else if (strcmp(argv[i], "--saida") == 0 && i + 1 < argc) {
            cfg.diretorio_saida = argv[++i];
        } else if (strcmp(argv[i], "--sem-sensibilidade") == 0) {
            cfg.executar_sensibilidade = 0;
        } else if (strcmp(argv[i], "--perda") == 0 && i + 1 < argc) {
            cfg.perda_forcada = atof(argv[++i]);
        } else if (strcmp(argv[i], "--degradados") == 0 && i + 1 < argc) {
            cfg.degradados_forcados = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--perturbacao") == 0 && i + 1 < argc) {
            cfg.perturbacao_forcada = atof(argv[++i]);
        } else {
            uso(argv[0]);
            return 2;
        }
        i++;
    }

    if (cfg.episodios < 10 || cfg.episodios > 100000) {
        fprintf(stderr, "Numero de episodios fora do intervalo [10,100000].\n");
        return 2;
    }
    printf("simulacao_inicio episodios=%d semente=%lu\n",
           cfg.episodios, cfg.semente);
    i = executar_experimento(&cfg);
    if (i != 0) {
        fprintf(stderr, "simulacao_falhou codigo=%d\n", i);
        return i;
    }
    printf("simulacao_concluida\n");
    return 0;
}
