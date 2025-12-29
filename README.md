# Projeto Eletronic Drum ESP32

Este projeto implementa um sistema de bateria eletrônica usando ESP32, com pads sensíveis à força (piezo) e envio de eventos MIDI. Cada pad é lido via ADC, e a velocidade da batida é calculada com base no valor máximo registrado do sensor.

## Funcionalidades

- Leitura de pads via ADC (piezo)
- Comunicação MIDI
- Suporte a todas as peças da bateria
- Sistema de filas FreeRTOS para envio de eventos MIDI
- Sistema web para ajustar cada pad

## Autor

Sérgio Teixeira Pimenta


# Arquitetura 

O firmware responsável pela leitura de piezos e eventos MIDI roda **integralmente no ESP32** e é organizado em **módulos bem definidos**, cada um com uma **responsabilidade clara**. Um
 **ESP-01** é opcional para interface web (parsing, JSON e UI).

 # Estrutura de Módulos

## controller_uart
**Camada responsável pela comunicação via UART.**

### Função geral
- Receber comandos externos
- Expor o estado interno do sistema
- **Não executa lógica de funcionamento**

### Camadas Esp32 e suas Responsabilidades

### enginecontroller 
**Responsabilidade:**  
- Entregar todo o estado do engine  
- Enviar dumps em texto simples via UART  
- Permitir consumo por outro dispositivo.


### padcontroller
**Responsabilidade:**  
- Receber comandos de configuração via UART  
- Setar propriedades dos pads no engine  

**Exemplos:**  
- Threshold  
- Sensibilidade  
- Nota MIDI  
- Retrigger  
- Peak hold  

## engine
**Responsável pelas funções reais das peças.**

**Função:**  
- Implementar o comportamento do instrumento  
- Carregar parâmetros  
- Processar sinais dos pads  
- Gerar eventos MIDI  

**Exemplo:** `pad.c` contendo:  
- Carregamento de parâmetros  
- Detecção de hit  
- Cálculo de velocity  
- Disparo de eventos  

## hardware
**Responsável por abstrair as camadas de hardware.**

**Função:**  
- Leitura de sensores  
- Acesso a ADC, GPIO, UART  
- Inicialização de periféricos  
- Interface entre engine/process e ESP-IDF  

## model
**Responsável pelos modelos de dados.**

**Função:**  
- Definir structs  
- Padronizar tipos compartilhados  

## process
**Responsável pela execução assíncrona e temporal.**

**Função:**  
- Timers  
- Timer de leitura de sensores  
- Verificação contínua de entradas  
- Garante funcionamento em tempo real  

## queue
**Responsável pela fila de eventos MIDI.**
**Função:**  
- Enfileirar eventos gerados pelo engine  
- Desacoplar geração e envio  
- Garantir ordem e não bloqueio  

## storage
**Responsável pela persistência de estado.**

**Função:**  
- Salvar configurações na NVS  
- Restaurar estado no boot  
- Persistir parâmetros dos pads  


## Integração com ESP-01 (Interface Web)
- **Comunicação via UART**  
- ESP32 envia dumps simples  

**ESP-01:**  
- Faz parsing  
- Constrói JSON  
- Exibe interface web  


## Protocolo de Comunicação

### Comando: GET ALL (estado completo)

## Objetivo
Solicitar o estado completo do engine, incluindo todas as peças e seus parâmetros atuais.

## Comando enviado via UART
GET ALL

## Resposta do ESP32 (dump de estado)

### Formato:
```text
BEGIN
PAD <id> <name> <threshold> <note> <sensitivity> <peak_hold> <retrigger>
PAD <id> <name> <threshold> <note> <sensitivity> <peak_hold> <retrigger>
END
```

## Comando: SET PAD

**Usado para alterar um parâmetro de um pad específico.**

### Sintaxe
```text
SET PAD <id> <PARAM> <value>
```

### Parâmetros aceitos
- THRESHOLD
- NOTE
- SENSITIVITY
- PEAK_HOLD
- RETRIGGER

### Exemplo

```text
SET PAD 1 THRESHOLD 3200
```