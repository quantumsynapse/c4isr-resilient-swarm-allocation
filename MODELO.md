# Especificação Matemática

## 1. Grafo de comunicação

Em cada cenário, o enxame é representado por

\[
\mathcal{G}=(\mathcal{V},\mathcal{E},P,D),
\]

onde \(\mathcal{V}\) contém 18 agentes, \(\mathcal{E}\) contém enlaces dirigidos, \(P_{ij}\) é a probabilidade de apagamento e \(D_{ij}\) é o atraso nominal.

Os enlaces iniciais seguem um grafo geométrico sintético. Cada agente recebe pelo menos dois vizinhos, evitando isolamento determinístico sem impor conectividade completa.

A confiabilidade de um caminho \(\pi\) é

\[
R(\pi)=\prod_{(i,j)\in\pi}(1-P_{ij}).
\]

O caminho até um agente de fusão é obtido por minimização de

\[
C(\pi)=\sum_{(i,j)\in\pi}-\log(1-P_{ij}).
\]

## 2. Estado do agente

O estado do agente \(i\) é

\[
x_i=[p_i,e_i,h_i,c_i,q_i,\tau_i,\ell_i,g_i],
\]

com posição \(p_i\), energia \(e_i\), saúde funcional \(h_i\), capacidades \(c_i\), qualidade da rede \(q_i\), confiança \(\tau_i\), carga \(\ell_i\) e preferência local \(g_i\).

As capacidades abstratas são EO, IR, RF, retransmissão, mapeamento e fusão.

## 3. Ações

Cada ação \(j\) é

\[
a_j=[p_j,v_j,d_j,\kappa_j,C_j,n_j,K_j],
\]

onde \(p_j\) é a prioridade, \(v_j\) é o valor informacional, \(d_j\) é o prazo, \(\kappa_j\) é a complexidade, \(C_j\) é o conjunto de capacidades requeridas, \(n_j\) é o tamanho mínimo da coalizão e \(K_j\) é o número de símbolos de origem.

## 4. Oferta

A oferta do agente \(i\) para a ação \(j\) combina valor, correspondência de capacidades, energia, saúde, distância, carga e confiabilidade da comunicação:

\[
b_{ij}=\alpha v_jp_jm_{ij}h_i
+\beta R_i
-\gamma d_{ij}
-\eta\ell_i
-\zeta(1-e_i)
-\xi c_i^{\mathrm{rele}}.
\]

O último termo preserva agentes estruturalmente importantes para retransmissão.

## 5. Perturbação de saída

O estado reportado é

\[
\widetilde y_i=y_i+\nu_i+d_i,
\]

onde \(\nu_i\) é ruído limitado e \(d_i\) é uma perturbação persistente aplicada a agentes degradados.

A confiança é atualizada por

\[
\tau_i(k+1)=0,88\tau_i(k)+0,12
\exp\left[-\frac{r_i(k)^2}{2\sigma^2}\right],
\]

onde \(r_i\) é o resíduo entre a saída reportada e a estimativa de consistência do simulador.

O mecanismo reduz a influência de ofertas incompatíveis. Ele não é um detector certificado e não substitui autenticação ou diagnóstico de falhas.

## 6. Leilão distribuído

Cada agente inicia com sua própria oferta e transmite apenas quando seu vencedor local muda. A propagação usa quatro rodadas e comparação determinística por maior oferta e menor identificador em caso de empate.

O vencedor global simulado é a moda das tabelas locais. A ação é reatribuída quando não há maioria mínima de 42% dos agentes elegíveis.

No mecanismo robusto, ofertas são limitadas por uma faixa plausível e ponderadas pela confiança.

## 7. Codificação linear aleatória

Para \(K\) símbolos de origem, cada símbolo de reparo é

\[
z_m=\sum_{k=1}^{K}c_{mk}s_k,
\qquad c_{mk}\in GF(2^8).
\]

A decodificação ocorre quando a matriz dos coeficientes recebidos atinge posto \(K\).

A redundância é

\[
\rho=\operatorname{sat}
\left[
0,12+1,30(1-R)+0,22p_j+0,12(1-\tau_i),
0,18,1,45
\right].
\]

O número de transmissões é

\[
N=\lceil K(1+\rho)\rceil.
\]

A estratégia central utiliza margem adicional de 0,10 para funcionar como referência superior sob o mesmo modelo de canal.

## 8. Métrica principal

Uma ação é fisicamente executada quando a coalizão atende capacidades, energia, saúde e prazo de execução.

Uma ação é informacionalmente concluída somente quando seus símbolos são reconstruídos por um agente de fusão antes do prazo.

O índice de continuidade é

\[
J_c=0,50V+0,25I+0,15C+0,10(1-Q),
\]

onde \(V\) é o valor entregue, \(I\) é a conclusão informacional, \(C\) é a conectividade e \(Q\) é a necessidade de reatribuição.

## 9. Hipótese

A hipótese testada é

\[
H_1:\quad E[I_{\mathrm{RLNC}}-I_{\mathrm{robusto}}]>0.
\]

A análise utiliza diferenças pareadas por cenário e intervalo normal de 95%. A simulação fornece evidência computacional sob o modelo; não constitui prova universal sobre sistemas reais.
