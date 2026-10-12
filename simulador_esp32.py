import os
import pty
import time
import json

# Cria um par de portas série virtuais (master/slave) no Linux
master, slave = pty.openpty()
nome_porta = os.ttyname(slave)

print(f"Simulador ESP32 a correr.")
print(f"-> Altere a constante PORTA_SERIAL no seu datalogger.py para: '{nome_porta}'")
print("A enviar pacotes de telemetria virtuais...\n")

rpm_simulado = 1000
try:
    while True:
        # Varia o RPM para simular a aceleração do motor
        rpm_simulado = (rpm_simulado + 150) % 6000
        temp_simulada = 85 + (rpm_simulado // 1000)

        # Empacota os dados (Engenharia reversa da descompressão)
        payload = [
            (rpm_simulado >> 8) & 0xFF,
            rpm_simulado & 0xFF,
            temp_simulada,
            0, 0, 0, 0, 0
        ]

        pacote = {"id": "0x1A4", "dlc": 8, "data": payload}
        linha_json = json.dumps(pacote) + "\n"

        # Escreve a linha na porta virtual
        os.write(master, linha_json.encode('utf-8'))
        time.sleep(0.1)  # Simula uma frequência de 10Hz

except KeyboardInterrupt:
    print("\nSimulação terminada.")