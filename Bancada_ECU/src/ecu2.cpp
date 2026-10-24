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
        Serial.println("[ECU-2] Erro ao inicializar MCP2515. Tentando novamente...");
        delay(500);
    }

    /*
    Filtro de ID: Aceita apenas o ID 0x000C
    Mascara 0x07FF: Exige os 11 bits
    FIltro 0x000C: Apenas o ID 0x000C é aceito
    */
    CAN.init_Mask(0, 0, 0x07FF);
    CAN.init_Filt(0, 0, 0x000C);

    CAN.setMode(MCP_NORMAL);
    Serial.println("[ECU-2] MCP2515 Inicializado com Sucesso!");
}

void loop(){
    // Pacote recebido
    byte dadosRec[8] = {0};
    unsigned long idRec;
    byte lenRec;
    int valorRec = 0;

    if(CAN.checkReceive() == CAN_MSGAVAIL){
        CAN.readMsgBuf(&idRec, &lenRec, dadosRec);

        // Reconstroi o inteiro a partir dos 2 primeiros bytes
        // Desloca dados[1] (MSB) 8 bits para a esquerda e junta com dados[0] (LSB)
        valorRec = (int)dadosRec[0] | ((int)dadosRec[1] << 8);

        // Exibe os dados no Serial Monitor
        Serial.print("[ECU-2] Pacote Recebido | ID: 0x");
        Serial.print(idRec, HEX);
        Serial.print(" | DLC: ");
        Serial.print(lenRec);
        Serial.print(" | Valor recebido: ");
        Serial.print(valorRec);
        Serial.println();

        // Envia um pacote quando o valor recebido for par
        byte dadosEnv[8] = {0};

        dadosEnv[0] = valorRec & 0xFF; // Isola os bits menos significativos (LSB)
        dadosEnv[1] = (valorRec >> 8) & 0xFF; // Isola os bits mais significativos (MSB)

        if(valorRec % 2 == 0 && valorRec != 0){

            byte statusB = CAN.sendMsgBuf(0x000B, 0, 8, dadosEnv);

            if(statusB == CAN_OK){
                Serial.print("[ECU-2] Dados enviados com sucesso! Valor: ");
                Serial.println(valorRec);
            }else{
                Serial.println("[ECU-2] Falha ao enviar dados");
            }
        }
    }
}