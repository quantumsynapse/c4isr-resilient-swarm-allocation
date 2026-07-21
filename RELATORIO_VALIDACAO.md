# Relatório de Validação

## 1. Ambiente

```text
linguagem        ISO C89/C90
compilador       GCC
opções           -std=c89 -pedantic -Wall -Wextra -O2
bibliotecas      libc e libm
sistema-alvo     Debian GNU/Linux
```

## 2. Compilação

Todos os módulos foram compilados sem advertências.

## 3. Testes funcionais

```text
teste_gf256=aprovado
teste_basico=aprovado
teste_reprodutibilidade=aprovado
teste_resultados=aprovado
```

O teste GF(2^8) verificou:

1. \(a\cdot a^{-1}=1\) para os 255 elementos não nulos;
2. posto máximo da matriz identidade 12 por 12;
3. decodificação determinística sem perdas;
4. falha determinística com perda total.

## 4. Memória e comportamento indefinido

Uma execução reduzida de 200 cenários foi compilada com AddressSanitizer e UndefinedBehaviorSanitizer.

Não foram detectados:

```text
acessos fora dos limites
uso após liberação
vazamentos
operações indefinidas sinalizadas
```

O programa não realiza alocação dinâmica durante a simulação principal.

## 5. Dados

Foram validados 3.000 cenários, 54.000 estados de agentes e 108.000 ações.

```text
cenários inválidos       0
NaN                       0
infinito                  0
capacidade sem cobertura  0
falta de agentes de fusão 0
```

## 6. Reprodutibilidade

Duas execuções independentes com a mesma semente produziram arquivos CSV idênticos byte a byte.

## 7. Hipótese

Os limites inferiores dos intervalos de confiança de 95% foram positivos para:

```text
conclusão informacional
valor entregue
continuidade
```

O programa marcou `hipotese_suportada=sim`.

## 8. Limitação da validação

A validação confirma consistência do software e do experimento sintético. Ela não certifica aeronavegabilidade, segurança funcional, segurança cibernética, robustez RF ou desempenho operacional.
