#include <Arduino.h>
#include <Wire.h>
#include <QMC5883LCompass.h>
#include <WiFi.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

// Hardware Configuration
const int SOS_BUTTON_PIN = 15;    // GPIO for SOS button
const int VIBRATION_MOTOR_PIN = 4; // GPIO for vibration motor

// WiFi Configuration
const char* ssid = "Boys Hostel";
const char* password = "DS1e@SP78";
const uint16_t port = 12345;       // D1's TCP port
IPAddress D1_IP(192, 168, 15, 201); // UPDATE WITH D1's ACTUAL IP

// BLE Configuration
const char* DEVICE_NAME = "D2";

// Global Objects
QMC5883LCompass compass;
WiFiClient client;
BLEServer *pServer;
BLEService *pService;
BLECharacteristic *pChar;
BLEAdvertising *pAdvertising;

void setup() {
  Serial.begin(115200);
  
  // Initialize I/O Pins
  pinMode(SOS_BUTTON_PIN, INPUT_PULLUP);
  pinMode(VIBRATION_MOTOR_PIN, OUTPUT);
  digitalWrite(VIBRATION_MOTOR_PIN, LOW);

  // Initialize Compass
  Wire.begin(21, 22); // SDA, SCL
  compass.init();
  Serial.println("Compass initialized");

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected! IP: " + WiFi.localIP().toString());

  // Initialize BLE Advertiser
  BLEDevice::init(DEVICE_NAME);
  pServer = BLEDevice::createServer();
  pService = pServer->createService(BLEUUID("1234"));
  pChar = pService->createCharacteristic(
    BLEUUID("ABCD"),
    BLECharacteristic::PROPERTY_READ
  );
  pChar->setValue("Distance");
  pService->start();
  pAdvertising = pServer->getAdvertising();
  pAdvertising->addServiceUUID(pService->getUUID());
  pAdvertising->start();
  Serial.println("BLE Advertising started");
}

void loop() {
  // 1. Read compass heading
  compass.read();
  int selfHeading = compass.getAzimuth();
  Serial.print("Heading: ");
  Serial.print(selfHeading);
  Serial.println("°");

  // 2. Get MAC address
  String macAddress = WiFi.macAddress();
  macAddress.replace(":", "");

  // 3. Connect to D1 and send data
  if (!client.connected()) {
    if (client.connect(D1_IP, port)) {
      Serial.println("Connected to D1 server");
    }
  }

  if (client.connected()) {
    String packet = macAddress + ",," + String(selfHeading) + "\n";
    client.print(packet);
    Serial.println("Sent: " + packet);
  } else {
    Serial.println("Connection to D1 failed");
  }

  // 4. Check SOS button
  if (digitalRead(SOS_BUTTON_PIN) == LOW) {
    digitalWrite(VIBRATION_MOTOR_PIN, HIGH); // Vibrate on press
    Serial.println("SOS button pressed!");
    
    if (client.connected()) {
      client.print("SOS," + macAddress + "\n");
      Serial.println("Sent SOS signal");
    }
    delay(1000); // Debounce and tactile feedback
    digitalWrite(VIBRATION_MOTOR_PIN, LOW);
  }

  delay(1000); // Main loop delay
}