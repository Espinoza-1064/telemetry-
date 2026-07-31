import socket
import json

ESP32_IP = "192.168.4.1"
UDP_PORT = 4210

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind(("", UDP_PORT))

sock.sendto(b"hello", (ESP32_IP, UDP_PORT))
print(f"Sent hello to {ESP32_IP}:{UDP_PORT}, waiting for telemetry...")

while True:
    data, addr = sock.recvfrom(1024)
    try:
        telemetry = json.loads(data.decode())
        accel = telemetry.get("accel", {})
        gyro = telemetry.get("gyro", {})
        gps = telemetry.get("gps", {})

        print(
            f"Accel: x={accel.get('x'):.2f} y={accel.get('y'):.2f} z={accel.get('z'):.2f} | "
            f"Gyro: x={gyro.get('x'):.2f} y={gyro.get('y'):.2f} z={gyro.get('z'):.2f} | "
            f"GPS: {gps}"
        )
    except json.JSONDecodeError:
        print(f"Received non-JSON data: {data}")