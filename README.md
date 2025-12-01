# Projeto Eletronic Drum ESP32

Este projeto implementa um sistema de bateria eletrônica usando ESP32, com pads sensíveis à força (piezo) e envio de eventos MIDI. Cada pad é lido via ADC, e a velocidade da batida é calculada com base no valor máximo registrado do sensor.

## Funcionalidades

- Leitura de pads via ADC (piezo)
- Comunicação MIDI
- Suporte a todas as peças da bateria
- Sistema de filas FreeRTOS para envio de eventos MIDI
- Sistema para ajustar sensibilidade de cada pad

## Autor

Sérgio Teixeira Pimenta