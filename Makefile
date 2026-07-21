CC = gcc
CFLAGS = -std=c89 -pedantic -Wall -Wextra -O2 -Iinclude
LDFLAGS = -lm

PROGRAMA = simulador_c4isr
FONTES = src/aleatorio.c src/util.c src/grafo.c src/cenario.c \
         src/gf256.c src/simulacao.c src/metricas.c src/experimento.c \
         src/principal.c
OBJETOS = $(FONTES:.c=.o)

all: $(PROGRAMA)

$(PROGRAMA): $(OBJETOS)
	$(CC) $(CFLAGS) -o $(PROGRAMA) $(OBJETOS) $(LDFLAGS)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

simular: $(PROGRAMA)
	mkdir -p resultados
	./$(PROGRAMA) --episodios 3000 --semente 1995 --saida resultados

simular-rapido: $(PROGRAMA)
	mkdir -p resultados/teste
	./$(PROGRAMA) --episodios 80 --semente 1995 \
		--saida resultados/teste --sem-sensibilidade

validar: clean all
	mkdir -p resultados
	./$(PROGRAMA) --episodios 3000 --semente 1995 --saida resultados
	sh testes/teste_gf256.sh
	sh testes/teste_basico.sh
	sh testes/teste_reprodutibilidade.sh
	sh testes/teste_resultados.sh

clean:
	rm -f $(OBJETOS) $(PROGRAMA) testes/teste_gf256
	rm -rf resultados/teste resultados/reprodutibilidade_a \
		resultados/reprodutibilidade_b

.PHONY: all simular simular-rapido validar clean
