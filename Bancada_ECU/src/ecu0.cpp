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

    while(CAN.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) != CAN_OK){
        Serial.println("[ECU-0] Erro ao inicializar MCP2515. Tentando novamente...");
        delay(500);
    }

    CAN.setMode(MCP_NORMAL);
    Serial.println("[ECU-0] MCP2515 Inicializado com Sucesso!");
}

void loop(){
    byte dados[8];

    dados[0] = contador & 0xFF; // Isola os bits menos significativos (LSB)
    dados[1] = (contador >> 8) & 0xFF; // Isola os bits mais significativos (MSB)
    // Preenche os bytes restantes com 0
    dados[2] = 0;
    dados[3] = 0;
    dados[4] = 0;
    dados[5] = 0;
    dados[6] = 0;
    dados[7] = 0;

    // Envia o pacote: ID 0x0A, Standard ID (0), DLC (8 bytes)
    byte status = CAN.sendMsgBuf(0x0A, 0, 8, dados);

    // Confirma a transmissao
    if(status == CAN_OK){
        Serial.print("[ECU-0] Dados enviados com sucesso! Valor: ");
        Serial.println(contador);
    }else{
        Serial.println("[ECU-0] Falha ao enviar dados");
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