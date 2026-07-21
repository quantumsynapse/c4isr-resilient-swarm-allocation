#!/bin/sh
set -eu

gcc -std=c89 -pedantic -Wall -Wextra -O2 -Iinclude \
    testes/teste_gf256.c src/gf256.c src/aleatorio.c -lm \
    -o testes/teste_gf256
./testes/teste_gf256
rm -f testes/teste_gf256
