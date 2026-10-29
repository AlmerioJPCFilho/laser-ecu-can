#ifndef CAN_DEFS_H
#define CAN_DEFS_H

#include <mcp2515.h>

// Pino SPI Chip Select padrão na sua placa
#define CAN_CS_PIN 10

// Velocidade e Cristal do MCP2515 (verifique se o cristal da sua placa é 8MHz ou 16MHz)
#define CAN_SPEED CAN_500KBPS
#define CAN_CLOCK MCP_8MHZ

// Dicionário de IDs da Bancada
#define CAN_ID_PAINEL_BOTOES 0x100

// Pinos locais das ECUs
#define PIN_VERDE   2
#define PIN_AMARELO 3

#endif