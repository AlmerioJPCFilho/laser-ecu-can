#include <Arduino.h>
#include <SPI.h>
#include "can_defs.h"

MCP2515 mcp2515(CAN_CS_PIN);
struct can_frame canMsg;

void setup() {
  Serial.begin(115200);
  pinMode(PIN_VERDE, OUTPUT);
  pinMode(PIN_AMARELO, OUTPUT);

  SPI.begin();
  mcp2515.reset();
  mcp2515.setBitrate(CAN_SPEED, CAN_CLOCK);
  mcp2515.setNormalMode();

  Serial.println("{\"status\":\"ECU LEDs Pronta\"}");
}

void loop() {
  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {
    if (canMsg.can_id == CAN_ID_PAINEL_BOTOES && canMsg.can_dlc >= 2) {
      digitalWrite(PIN_VERDE, canMsg.data[0] ? HIGH : LOW);
      digitalWrite(PIN_AMARELO, canMsg.data[1] ? HIGH : LOW);

      Serial.print("{\"id\":\"0x100\",\"verde\":");
      Serial.print(canMsg.data[0]);
      Serial.print(",\"amarelo\":");
      Serial.print(canMsg.data[1]);
      Serial.println("}");
    }
  }
}