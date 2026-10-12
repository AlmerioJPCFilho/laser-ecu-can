#include <Arduino.h>
#include <SPI.h>
#include <mcp2515.h>

#define CS_PIN 5 

MCP2515 mcp2515(CS_PIN);
struct can_frame canMsg;

void setup() {
  Serial.begin(115200);
  SPI.begin();
  
  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ);
  
  // O modo Listen Only é ideal para o Gateway, pois apenas escuta 
  // a rede sem interferir (não envia bits de ACK)
  mcp2515.setListenOnlyMode(); 
  
  Serial.println("{\"status\": \"Gateway CAN iniciado em modo Listen Only\"}");
}

void loop() {
  // Verifica se há novas mensagens no buffer do MCP2515
  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {
    
    // Inicia a formatação do JSON
    Serial.print("{\"id\":\"0x");
    Serial.print(canMsg.can_id, HEX);
    Serial.print("\", \"dlc\":");
    Serial.print(canMsg.can_dlc);
    Serial.print(", \"data\":[");
    
    // Itera sobre os bytes de dados (Payload)
    for (int i = 0; i < canMsg.can_dlc; i++) {
      Serial.print(canMsg.data[i]);
      if (i < (canMsg.can_dlc - 1)) {
        Serial.print(", ");
      }
    }
    Serial.println("]}");
  }
}