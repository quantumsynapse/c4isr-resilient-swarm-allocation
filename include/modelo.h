#ifndef MODELO_H
#define MODELO_H

#include <stdio.h>

#define MAX_AGENTES 18
#define MAX_ACOES 36
#define MAX_CAPACIDADES 6
#define MAX_SIMBOLOS 12
#define NUM_ESTRATEGIAS 5
#define MAX_CAMINHO 32

#define CAP_EO       1U
#define CAP_IR       2U
#define CAP_RF       4U
#define CAP_RELE     8U
#define CAP_MAPA    16U
#define CAP_FUSAO   32U

#define ESTRATEGIA_CENTRAL 0
#define ESTRATEGIA_GULOSA 1
#define ESTRATEGIA_LEILAO 2
#define ESTRATEGIA_ROBUSTA 3
#define ESTRATEGIA_PROPOSTA 4

typedef struct {
    unsigned long estado;
    int possui_gaussiana;
    double gaussiana_reserva;
} Gerador;

typedef struct {
    int id;
    double x;
    double y;
    double energia;
    double saude;
    double confianca;
    double carga;
    double perda_base;
    double vies_objetivo;
    unsigned int capacidades;
    int degradado;
    double perturbacao_persistente;
} Agente;

typedef struct {
    int id;
    double x;
    double y;
    double prioridade;
    double valor;
    double prazo;
    double complexidade;
    unsigned int capacidades_requeridas;
    int agentes_minimos;
    int simbolos;
} Acao;

typedef struct {
    int adj[MAX_AGENTES][MAX_AGENTES];
    double perda[MAX_AGENTES][MAX_AGENTES];
    int atraso[MAX_AGENTES][MAX_AGENTES];
} Grafo;

typedef struct {
    int numero_agentes;
    int numero_acoes;
    double perda_global;
    int numero_degradados;
    double escala_perturbacao;
    Agente agentes[MAX_AGENTES];
    Acao acoes[MAX_ACOES];
    Grafo grafo;
} Cenario;

typedef struct {
    double execucao_fisica;
    double conclusao_informacional;
    double valor_entregue;
    double cumprimento_prazo;
    double energia_consumida;
    double pacotes_transmitidos;
    double conflitos_consenso;
    double reatribuicoes;
    double aceitacoes_degradadas;
    double conectividade;
    double atraso_decodificacao;
    double continuidade;
} MetricaEpisodio;

typedef struct {
    double soma;
    double soma_quadrados;
    long n;
} Acumulador;

typedef struct {
    Acumulador execucao_fisica;
    Acumulador conclusao_informacional;
    Acumulador valor_entregue;
    Acumulador cumprimento_prazo;
    Acumulador energia_consumida;
    Acumulador pacotes_transmitidos;
    Acumulador conflitos_consenso;
    Acumulador reatribuicoes;
    Acumulador aceitacoes_degradadas;
    Acumulador conectividade;
    Acumulador atraso_decodificacao;
    Acumulador continuidade;
} EstatisticaEstrategia;

typedef struct {
    double media;
    double desvio;
    double erro_padrao;
    double limite_inferior;
    double limite_superior;
} ResumoEstatistico;

typedef struct {
    int episodios;
    unsigned long semente;
    double perda_forcada;
    int degradados_forcados;
    double perturbacao_forcada;
    int executar_sensibilidade;
    const char *diretorio_saida;
} ConfiguracaoExecucao;

void rng_inicializar(Gerador *g, unsigned long semente);
unsigned long rng_proximo(Gerador *g);
double rng_uniforme(Gerador *g);
int rng_inteiro(Gerador *g, int minimo, int maximo);
double rng_normal(Gerador *g);
int rng_poisson(Gerador *g, double lambda, int limite);

double limitar(double valor, double minimo, double maximo);
double distancia(double x1, double y1, double x2, double y2);
int contar_bits(unsigned int valor);
int valor_finito(double valor);
const char *nome_estrategia(int estrategia);

void gerar_cenario(Cenario *cenario, Gerador *g, double perda_forcada,
                   int degradados_forcados, double perturbacao_forcada);
double validar_cenario(const Cenario *cenario, FILE *saida);

void construir_grafo(Cenario *cenario, Gerador *g);
double fracao_maior_componente(const Cenario *cenario);
int melhor_caminho_fusao(const Cenario *cenario, int origem,
                         double *probabilidade, double *atraso_medio,
                         int *saltos);
double centralidade_grau(const Cenario *cenario, int agente);

unsigned char gf256_multiplicar(unsigned char a, unsigned char b);
unsigned char gf256_inverter(unsigned char a);
int gf256_posto(unsigned char matriz[MAX_SIMBOLOS][MAX_SIMBOLOS],
                int linhas, int colunas);
int transmissao_rlnc(int simbolos, int transmissoes,
                     double probabilidade_entrega, double atraso_medio,
                     double prazo_restante, Gerador *g,
                     double *atraso_observado, int *recebidos);
int transmissao_sem_codigo(int simbolos, int transmissoes,
                           double probabilidade_entrega,
                           double atraso_medio, double prazo_restante,
                           Gerador *g, double *atraso_observado,
                           int *recebidos);

void executar_estrategia(const Cenario *cenario_base, int estrategia,
                         unsigned long semente, MetricaEpisodio *metrica);

void acumulador_inicializar(Acumulador *a);
void acumulador_adicionar(Acumulador *a, double valor);
ResumoEstatistico acumulador_resumir(const Acumulador *a);
void estatistica_inicializar(EstatisticaEstrategia *e);
void estatistica_adicionar(EstatisticaEstrategia *e,
                           const MetricaEpisodio *m);

int executar_experimento(const ConfiguracaoExecucao *configuracao);

#endif
