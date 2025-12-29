# Projeto Eletronic Drum ESP32

Este projeto implementa um sistema de bateria eletrônica usando ESP32, com pads sensíveis à força (piezo) e envio de eventos MIDI. Cada pad é lido via ADC, e a velocidade da batida é calculada com base no valor máximo registrado do sensor.

## Funcionalidades

- Leitura de pads via ADC (piezo)
- Comunicação MIDI
- Suporte a todas as peças da bateria
- Sistema de filas FreeRTOS para envio de eventos MIDI
- Sistema para ajustar sensibilidade de cada pad



# UART Command Protocol – Electronic Drum (ESP32)

Este documento descreve o protocolo de comunicação via UART utilizado para configurar parâmetros do módulo de bateria eletrônica baseado em ESP32.

O protocolo é **texto plano**, orientado a comandos, e suporta **envio em lote** (múltiplos comandos em uma única transmissão).

---

## Transporte

- Interface: UART
- Codificação: ASCII
- Final de linha: `\n`
- Baudrate: definido na inicialização do UART
- Buffer RX: processado por linhas

---

## Formato Geral

Cada comando deve ocupar **uma linha**:

<COMANDO>\n  
text

Exemplo:

SET PAD 1 THRESHOLD 3000

---

## Comando SET

Usado para configurar parâmetros de um pad específico.

### Sintaxe

SET PAD \<id> \<PARAM> \<value>

### Campos

| Campo   | Descrição               |
|---------|-------------------------|
| `id`    | Identificador do pad    |
| `PARAM` | Nome do parâmetro       |
| `value` | Valor inteiro           |

---

## Parâmetros suportados

| Parâmetro     | Descrição                           |
|---------------|-------------------------------------|
| `THRESHOLD`   | Valor mínimo para detecção do hit   |
| `NOTE`        | Nota MIDI associada ao pad          |
| `SENSITIVITY` | Sensibilidade do piezo              |
| `PEAK_HOLD`   | Tempo de hold do pico               |
| `RETRIGGER`   | Tempo de bloqueio para retrigger    |

---

## Exemplos

### Comando único

SET PAD 1 NOTE 38

### Envio em lote

SET PAD 1 THRESHOLD 3000  
SET PAD 1 SENSITIVITY 100  
SET PAD 1 NOTE 38

---

## Processamento

- Os dados recebidos via UART são separados por `\n`
- Cada linha é tratada como um comando independente
- Espaços e caracteres de fim de linha (`\r`, `\n`) são normalizados
- Caso qualquer comando do lote falhe, o processamento retorna erro

---

## Resposta

Após o processamento do comando ou lote:

- `ACK\n` → todos os comandos processados com sucesso
- `ERR\n` → erro em um ou mais comandos

---

## Observações

- O protocolo é **case-sensitive**
- Parâmetros desconhecidos resultam em erro
- Pads inexistentes resultam em erro
- O protocolo foi projetado para ser simples, determinístico e facilmente extensível


## GET ENGINE — Contrato de Comunicação (UART)

### Comando
GET ENGINE

### Resposta
BEGIN  
PAD <id> <name> <threshold> <note> <sensitivity> <peak_hold> <retrigger>  
PAD <id> <name> <threshold> <note> <sensitivity> <peak_hold> <retrigger>  
END

### Regras
- `BEGIN` e `END` são obrigatórios.
- 1 linha = 1 objeto persistente.
- Ordem dos campos é fixa.
- Apenas dados persistentes (ex: `pad_persist_t`).
- Usado no boot da aplicação web (ESP01).


## Autor

Sérgio Teixeira Pimenta