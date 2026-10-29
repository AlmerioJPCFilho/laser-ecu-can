#include <Arduino.h>
#include <SPI.h>
#include "can_defs.h"

MCP2515 mcp2515(CAN_CS_PIN);
struct can_frame canMsg;

uint8_t ultimoVerde = 0;
uint8_t ultimoAmarelo = 0;

void setup() {
  Serial.begin(115200);
  pinMode(PIN_VERDE);
  pinMode(PIN_AMARELO);

  SPI.begin();
  mcp2515.reset();
  mcp2515.setBitrate(CAN_SPEED, CAN_CLOCK);
  mcp2515.setNormalMode();

  canMsg.can_id  = CAN_ID_PAINEL_BOTOES;
  canMsg.can_dlc = 2;

  Serial.println("ECU Botoes Iniciada!");
}

void loop() {
  uint8_t estadoVerde = (digitalRead(PIN_VERDE) == LOW) ? 1 : 0;
  uint8_t estadoAmarelo = (digitalRead(PIN_AMARELO) == LOW) ? 1 : 0;

  if (estadoVerde != ultimoVerde || estadoAmarelo != ultimoAmarelo) {
    ultimoVerde = estadoVerde;
    ultimoAmarelo = estadoAmarelo;

    canMsg.data[0] = estadoVerde;
    canMsg.data[1] = estadoAmarelo;

    mcp2515.sendMessage(&canMsg);
    delay(50); // Debounce
  }
}