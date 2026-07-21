# Alocação C4ISR Distribuída e Resiliente para Enxames UAS com RLNC Adaptativa

Desenvolvi uma simulação em C89/C90 para avaliar a atribuição distribuída de ações a um enxame UAS representado por agentes orientados a objetivos, sob perda de comunicação, topologia variável e perturbação de saída.

A formulação combina leilão distribuído, confiança residual, preservação de conectividade e Random Linear Network Coding sobre GF(2^8). Uma ação somente é considerada concluída quando foi executada e sua informação reconstruída por um nó de fusão dentro do prazo.

Em 3.000 cenários sintéticos por estratégia, totalizando 15.000 episódios-estratégia, o modelo proposto atingiu conclusão informacional de 0,8862 e valor entregue de 0,8880. Em relação ao leilão resiliente sem codificação, os ganhos relativos foram 21,78% e 21,85%, respectivamente. A diferença pareada em conclusão foi 0,1585, com intervalo de confiança de 95% entre 0,1554 e 0,1616.

O desempenho ficou a menos de 0,5 ponto percentual da referência centralizada com conhecimento global, ao custo de 35,24% mais símbolos-pacote que a referência distribuída robusta sem código.

O trabalho integra áreas nas quais tenho experiência, entre outras, incluindo sistemas aeronáuticos e não tripulados, teoria de grafos, processamento de sinais, controle, inteligência computacional e engenharia de software, com atuação em pesquisa e desenvolvimento desde 1995.

Todos os dados, agentes, ações, enlaces e resultados são integralmente sintéticos. O modelo é restrito a C4ISR não cinético e não contém seleção de alvos, armamentos ou parâmetros operacionais reais.
