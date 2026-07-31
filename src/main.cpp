#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <MPU6050_light.h>
#include <TinyGPS++.h>
#include <ArduinoJson.h>

const char* ssid = "drone_esp32";
const char* password = "Esp32203"; 

WiFiUDP udp;
const int udpPort = 4210;
IPAddress receiverIP; 
bool receiverKnown = false;

MPU6050 mpu(Wire);

TinyGPSPlus gps;
HardwareSerial GPSSerial(1);
const int GPS_RX_PIN = 16;
const int GPS_TX_PIN = 17;

unsigned long lastSend = 0;
const unsigned long sendInterval = 100; 

void setup() {
  Serial.begin(115200);
  
  WiFi.softAP(ssid, password, 6);
  Serial.println("Access Point started");
  Serial.print("AP IP Address: ");
  Serial.println(WiFi.softAPIP());

  udp.begin(udpPort);
  Serial.print("UDP listening/sending on port ");
  Serial.println(udpPort);


  Wire.begin();
  byte status = mpu.begin();
  if (status != 0) {
    Serial.println("MPU6050 not found");
  } else {
    Serial.println("Calibrating MPU6050, keep it still...");
    mpu.calcOffsets(); 
    Serial.println("MPU6050 initialized");
  }

  GPSSerial.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
  Serial.println("GPS serial started");
}

void loop() {
 
  while (GPSSerial.available() > 0) {
    gps.encode(GPSSerial.read());
  }

  
  int packetSize = udp.parsePacket();
  if (packetSize > 0) {
    receiverIP = udp.remoteIP();
    receiverKnown = true;
    Serial.print("Receiver registered: ");
    Serial.println(receiverIP);
  }

  mpu.update();

  if (receiverKnown && millis() - lastSend >= sendInterval) {
    lastSend = millis();

    JsonDocument doc;

    JsonObject accel = doc["accel"].to<JsonObject>();
    accel["x"] = mpu.getAccX();
    accel["y"] = mpu.getAccY();
    accel["z"] = mpu.getAccZ();

    JsonObject gyro = doc["gyro"].to<JsonObject>();
    gyro["x"] = mpu.getGyroX();
    gyro["y"] = mpu.getGyroY();
    gyro["z"] = mpu.getGyroZ();

    JsonObject gpsData = doc["gps"].to<JsonObject>();
    if (gps.location.isValid()) {
      gpsData["lat"] = gps.location.lat();
      gpsData["lon"] = gps.location.lng();
      gpsData["alt"] = gps.altitude.meters();
      gpsData["sats"] = gps.satellites.value();
    } else {
      gpsData["fix"] = false;
    }

    doc["millis"] = millis();

    char buffer[256];
    size_t len = serializeJson(doc, buffer);

    udp.beginPacket(receiverIP, udpPort);
    udp.write((const uint8_t*)buffer, len);
    udp.endPacket();
  }
}