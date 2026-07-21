# Análise Técnico-Científica

## 1. Questão

O experimento procura determinar se a conclusão de uma ação distribuída deve incluir, além da execução física, a reconstrução tempestiva da informação produzida.

A hipótese estrutural é que a alocação ótima não depende apenas da capacidade do agente. Ela depende da capacidade conjunta de observar, executar, transportar, validar e reconstruir a informação.

## 2. Separação dos efeitos

A comparação entre o leilão distribuído e o leilão resiliente sem código mede principalmente o efeito da confiança residual e da consciência da comunicação.

A comparação entre o leilão resiliente sem código e a proposta mede principalmente o efeito da codificação linear, pois ambos usam a mesma lógica de ofertas, caminhos e redundância do plano de dados.

A comparação com a referência central mede a distância para uma política com conhecimento global dentro do próprio simulador.

## 3. Resultado principal

A conclusão informacional aumentou de 0,727750 para 0,886231 quando a repetição cíclica foi substituída por RLNC adaptativa.

A diferença pareada foi 0,158481, com intervalo de confiança de 95% [0,155356; 0,161607]. Como todo o intervalo é positivo e distante de zero, os resultados sustentam a hipótese dentro do modelo.

O tamanho de efeito pareado aproximado foi 1,81 desvios padrão para a conclusão informacional. O ganho não é apenas consequência do tamanho da amostra.

## 4. Mecanismo causal computacional

Na transmissão não codificada, cópias adicionais podem repetir símbolos já recebidos, enquanto outros permanecem ausentes. A conclusão exige cobertura de todos os símbolos.

Na RLNC, um símbolo recebido contribui quando sua linha aumenta o posto da matriz. Para uma janela pequena sobre GF(2^8), a probabilidade de dependência linear entre linhas aleatórias é baixa. O ganho observado decorre da redução de redundância não inovadora.

A alocação consciente da comunicação seleciona agentes com maior confiabilidade até fusão e evita consumir nós estruturalmente importantes para retransmissão. A confiança residual reduz a aceitação de ofertas artificialmente infladas.

Esses mecanismos atuam em níveis diferentes:

```text
confiança                 reduz informação de estado inconsistente
alocação consciente       reduz escolhas sem caminho informacional
eventos de comunicação    reduz transmissões de consenso desnecessárias
RLNC                      reduz perda de graus de liberdade no plano de dados
```

## 5. Custo

A proposta utilizou 351,98 símbolos-pacote por ação, contra 260,19 na política robusta sem código.

O aumento de 35,24% produziu ganho relativo de 21,78% em conclusão e 21,85% em valor entregue. Portanto, o mecanismo troca capacidade de comunicação por continuidade informacional.

O custo deve ser tratado como restrição explícita. Em enlaces estreitos, a política deve reduzir janela, densidade ou redundância. O simulador não sustenta a conclusão de que maior redundância é sempre preferível.

## 6. Regime de perda

A proposta apresentou queda ordenada de 0,895417 para 0,856852 quando a perda global aumentou de 0,05 para 0,35.

A comunicação aumentou de 268,21 para 503,78 símbolos-pacote por ação. Isso confirma que a robustez não é gratuita e que a adaptação responde ao canal.

A vantagem sobre a política robusta sem código permaneceu positiva em toda a faixa. Esse resultado é consistente com a hipótese de que o posto da matriz é uma medida operacional mais adequada do que o número bruto de pacotes recebidos.

## 7. Perturbação de saída

O leilão convencional aceitou agentes degradados em 34,06% das seleções. As políticas robustas reduziram esse valor para cerca de 0,7% no experimento principal.

Quando a amplitude da perturbação foi zero, a aceitação aumentou porque não existe informação observável suficiente para separar um agente degradado de um agente regular. Esse resultado é importante: nenhum filtro residual pode detectar uma falha que não produz resíduo observável.

A partir de amplitude 0,15, a aceitação caiu para aproximadamente 2,2%. O comportamento é coerente com a estrutura do observador de confiança.

## 8. Referência central

A proposta atingiu 0,886231 de conclusão, contra 0,890417 da referência central. A diferença foi inferior a meio ponto percentual.

Isso indica que, no modelo, a combinação de informação local, propagação por evento e codificação adaptativa recuperou grande parte do desempenho de uma política com conhecimento global.

Não se deve interpretar esse resultado como prova de equivalência entre controle distribuído e centralizado. Ambos compartilham as mesmas hipóteses, distribuições e modelo de canal.

## 9. Validade

Validade interna:

```text
compilação estrita sem advertências
0 cenários inválidos em 3.000
reprodutibilidade determinística
álgebra GF(2^8) testada para todos os elementos não nulos
AddressSanitizer sem erro em execução reduzida
UndefinedBehaviorSanitizer sem erro em execução reduzida
sensibilidade independente a perda e perturbação
```

Validade externa:

Os resultados não foram calibrados com telemetria, canal RF, plataforma ou missão real. A etapa seguinte cientificamente defensável seria Hardware-in-the-Loop com modelos de canal publicados, seguida por ensaio de voo em ambiente civil controlado.

## 10. Conclusão

A hipótese foi suportada computacionalmente: sob os cenários sintéticos avaliados, o acoplamento entre alocação resiliente e RLNC adaptativa aumentou a continuidade informacional de forma estatisticamente separável das referências distribuídas.

A contribuição não é um novo leilão isolado nem um novo código isolado. É a definição da ação concluída como um evento conjunto de execução e reconstrução da informação.
