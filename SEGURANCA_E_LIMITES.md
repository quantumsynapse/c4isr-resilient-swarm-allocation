# Segurança e Limites

## 1. Escopo permitido

O programa modela apenas ações não cinéticas:

```text
observação
reconhecimento
mapeamento
retransmissão
confirmação multissensorial
fusão de informação
recuperação de conectividade
```

Não existem modelos de armas, engajamento, seleção de alvos, terminal guidance, dano, carga útil ofensiva ou regras de emprego.

## 2. Dados

Não há dados reais, classificados, pessoais, operacionais ou proprietários.

Não há coordenadas geográficas, frequências, potência, alcance, assinatura, identificação de plataforma ou organização.

## 3. Rede e sistema local

O programa C:

```text
não abre sockets
não acessa a Internet
não lê variáveis de ambiente
não executa comandos
não acessa SSH
não solicita privilégios
não grava fora do diretório indicado
```

Os scripts removem apenas executáveis, objetos e diretórios de teste com nomes fixos dentro do projeto.

## 4. Criptografia

RLNC é codificação de apagamentos. Não fornece sigilo, autenticação ou integridade.

Uma implementação real deve utilizar protocolos criptográficos padronizados, gerenciamento de chaves independente e proteção dos parâmetros de codificação.

Este projeto não implementa criptografia própria.

## 5. Interpretação

Os termos C4ISR e UAS definem o domínio abstrato da pesquisa. Os resultados não autorizam uso operacional nem substituem ensaios, certificação, análise de segurança ou avaliação jurídica.
