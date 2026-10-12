import serial
import json
import time
import sys

# No Linux, o ESP32 costuma ser montado como ttyUSB0, ttyUSB1 ou ttyACM0
PORTA_SERIAL = '/dev/pts/4' 
BAUD_RATE = 115200

def decodificar_dados_motor(dados_payload):
    """
    Função auxiliar para traduzir os bytes puros de volta para grandezas físicas
    (Engenharia Reversa dos dados empacotados na ECU)
    """
    # O RPM foi enviado nos bytes 0 e 1 (High e Low)
    rpm = (dados_payload[0] << 8) | dados_payload[1]
    
    # A temperatura foi enviada no byte 2
    temperatura = dados_payload[2]
    
    return rpm, temperatura

def iniciar_gateway():
    try:
        # Inicia a ligação serial
        print(f"Tentando conectar ao Gateway na porta {PORTA_SERIAL} a {BAUD_RATE} bps...")
        conexao = serial.Serial(PORTA_SERIAL, BAUD_RATE, timeout=1)
        print("Conectado com sucesso! Aguardando telemetria...\n")
        print("-" * 50)

        while True:
            # Verifica se há bytes na fila de espera
            if conexao.in_waiting > 0:
                # Lê a linha até ao \n, decodifica e remove espaços em branco
                linha_bruta = conexao.readline().decode('utf-8', errors='ignore').strip()
                
                # Tenta converter a string pura para um dicionário Python (JSON)
                try:
                    pacote = json.loads(linha_bruta)
                    
                    # Verifica se é uma mensagem de telemetria CAN válida
                    if "id" in pacote and "data" in pacote:
                        can_id = pacote["id"]
                        payload = pacote["data"]
                        timestamp = time.strftime('%H:%M:%S.%f')[:-3]
                        
                        # Se for o ID da ECU do Motor (0x1A4), aplica a decodificação
                        if can_id == "0x1a4" or can_id == "0x1A4":
                            rpm, temp = decodificar_dados_motor(payload)
                            print(f"[{timestamp}] MOTOR  | RPM: {rpm:4d} | Temp: {temp} °C | (Raw: {payload})")
                        else:
                            # Para outras ECUs não mapeadas, exibe apenas o raw data
                            print(f"[{timestamp}] ID {can_id} | Payload: {payload}")
                    
                    # Mensagens de status do próprio ESP32
                    elif "status" in pacote:
                        print(f"[STATUS Gateway] {pacote['status']}")

                except json.JSONDecodeError:
                    # Se o ESP32 enviar mensagens de erro do sistema (não JSON), imprime como debug
                    if linha_bruta:
                        print(f"[DEBUG ESP32] {linha_bruta}")

    except serial.SerialException as e:
        print(f"\n[ERRO] Não foi possível abrir a porta {PORTA_SERIAL}.")
        print(f"Detalhes: {e}")
        print("Verifique se a porta está correta ou se o Monitor Serial do VS Code está aberto prendendo a porta.")
    except KeyboardInterrupt:
        print("\n\nEncerrando aquisição de dados...")
    finally:
        if 'conexao' in locals() and conexao.is_open:
            conexao.close()
            print("Porta serial fechada com segurança.")

if __name__ == '__main__':
    iniciar_gateway()