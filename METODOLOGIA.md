# Metodologia Experimental

## 1. Desenho

O experimento principal utiliza cenários comuns às cinco estratégias. A geometria, os agentes, as ações, as capacidades e as condições globais são idênticos em cada comparação.

A aleatoriedade interna de execução e comunicação recebe sementes determinísticas derivadas da semente do cenário e do identificador da estratégia.

## 2. Estratégias

A referência central conhece todo o cenário, escolhe ofertas com estado verdadeiro e utiliza RLNC.

A política gulosa escolhe o agente capaz mais próximo.

O leilão convencional utiliza ofertas reportadas, propagação local e transmissão não codificada.

O leilão resiliente limita ofertas, atualiza confiança por resíduo, considera a rota até fusão e utiliza repetição não codificada.

O modelo proposto mantém a alocação resiliente e substitui a repetição do plano de dados por RLNC sobre GF(2^8). O plano de controle usa uma segunda oportunidade somente em enlaces ou ações de maior risco.

## 3. Comunicação

O leilão transmite atualizações somente quando o vencedor local muda. Isso modela comunicação disparada por evento e evita retransmissão incondicional em todas as rodadas.

A contagem de pacotes representa símbolos-pacote por ação. Em tráfego multi-hop, cada salto é contado. O valor não representa bytes, frequência de rádio ou carga real de enlace.

## 4. Comparação codificada

A estratégia robusta e a proposta usam a mesma função de redundância para o plano de dados. A diferença principal é:

```text
robusta sem código     repetição cíclica dos K símbolos
proposta               K símbolos sistemáticos e reparos RLNC
```

A recepção não codificada exige pelo menos uma cópia de cada símbolo. A recepção RLNC exige posto K na matriz de coeficientes.

## 5. Estatística

Para cada métrica são calculados média, desvio amostral, erro padrão e intervalo de confiança de 95%.

A hipótese principal usa a diferença pareada entre a proposta e o leilão resiliente sem código.

O valor t apresentado é

\[
t=\frac{\overline d}{s_d/\sqrt n}.
\]

Ele é usado como diagnóstico de separação estatística, não como substituto de validação externa.

## 6. Convergência

As médias foram registradas em 300, 750, 1.500 e 3.000 cenários. A diferença entre 1.500 e 3.000 cenários para a conclusão da proposta foi aproximadamente 0,00055.

## 7. Sensibilidade

A perda global foi fixada em 0,05, 0,15, 0,25 e 0,35.

A perturbação persistente foi fixada em 0,00, 0,15, 0,30 e 0,45 com três agentes degradados.

Cada ponto de sensibilidade utilizou 600 cenários por estratégia.

## 8. Critério de suporte à hipótese

A hipótese é marcada como suportada somente quando os limites inferiores dos intervalos de 95% das diferenças em conclusão informacional, valor entregue e continuidade são positivos.
