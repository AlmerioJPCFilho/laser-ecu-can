#include <Arduino.h>
#include <SPI.h>
#include <mcp_can.h>

// pinos padrao
const int CS_PIN = 5;
const int SCK_PIN = 18;
const int MISO_PIN = 19;
const int MOSI_PIN = 23;

MCP_CAN CAN(CS_PIN);

void setup(){
    Serial.begin(115200);

    SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, CS_PIN);

    while(CAN.begin(MCP_STD, CAN_500KBPS, MCP_8MHZ) != CAN_OK){
        Serial.println("[ECU-1] Erro ao inicializar MCP2515. Tentando novamente...");
        delay(500);
    }

    /*
    Filtro de ID: Aceita apenas os IDs 0x000A e 0x000B
    Mascara 0x07FE: Exige os os 10 MSB
    Filtro 0x000A: IDs 0x000A e 0x000B sao identicos por causa da mascara
    */
    CAN.init_Mask(0, 0, 0x07FE);
    CAN.init_Filt(0, 0, 0x000A);

    CAN.setMode(MCP_NORMAL);
    Serial.println("[ECU-1] MCP2515 Inicializado com Sucesso!");
}

void loop(){
    byte dados[8] = {0};
    unsigned long id;
    byte len;

    if(CAN.checkReceive() == CAN_MSGAVAIL){
        CAN.readMsgBuf(&id, &len, dados);

        // Reconstroi o inteiro a partir dos 2 primeiros bytes
        // Desloca dados[1] (MSB) 8 bits para a esquerda e junta com dados[0] (LSB)
        int valorRecebido = (int)dados[0] | ((int)dados[1] << 8);

        // Exibe os dados no Serial Monitor
        Serial.print("[ECU-1] Pacote Recebido | ID: 0x");
        Serial.print(id, HEX);
        Serial.print(" | DLC: ");
        Serial.print(len);
        Serial.print(" | Valor recebido: ");
        Serial.print(valorRecebido);
        Serial.println();
    }
}