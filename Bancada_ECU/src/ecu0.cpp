#include <Arduino.h>
#include <SPI.h>
#include <mcp_can.h>

// pinos padrao
const int CS_PIN = 5;
const int SCK_PIN = 18;
const int MISO_PIN = 19;
const int MOSI_PIN = 23;

MCP_CAN CAN(CS_PIN);

int contador = 0;
int subindo = 1;

void setup(){
    Serial.begin(115200);

    SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, CS_PIN);

    while(CAN.begin(MCP_STD, CAN_500KBPS, MCP_8MHZ) != CAN_OK){
        Serial.println("[ECU-0] Erro ao inicializar MCP2515. Tentando novamente...");
        delay(500);
    }

    CAN.setMode(MCP_NORMAL);
    Serial.println("[ECU-0] MCP2515 Inicializado com Sucesso!");
}

void loop(){
    byte dados[8] = {0};
    dados[0] = contador & 0xFF; // Isola os bits menos significativos (LSB)
    dados[1] = (contador >> 8) & 0xFF; // Isola os bits mais significativos (MSB)

    // Envia os pacotes: ID 0x000A (ECU 1) e ID 0x000C (ECU 2)
    byte statusA = CAN.sendMsgBuf(0x000A, 0, 8, dados);
    byte statusC = CAN.sendMsgBuf(0x000C, 0, 8, dados);

    // Confirma a transmissao
    if(statusA == CAN_OK && statusC == CAN_OK){
        Serial.print("[ECU-0] Dados enviados com sucesso! Valor: ");
        Serial.println(contador);
    }else if(statusA != CAN_OK){
        Serial.println("[ECU-0] Falha ao enviar dados para ECU 1");
    }else if(statusC != CAN_OK){
        Serial.println("[ECU-0] Falha ao enviar dados para ECU 2");
    }

    if(subindo){
        contador += 10;
        if(contador >= 1023){
            contador = 1023;
            subindo = 0;
        }
    }else{
        contador -= 10;
        if(contador <= 0){
            contador = 0;
            subindo = 1;
        }
    }
    delay(100);
}