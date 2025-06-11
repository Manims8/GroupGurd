#include <WiFi.h>
#include <Wire.h>
#include <QMC5883LCompass.h>
#include <BLEDevice.h>
#include <BLEServer.h>

// WiFi Configuration
const char* ssid = "Mani";
const char* password = "12345678";
const uint16_t port = 12345;        // Port used by D1 and D3

// Receiver IP Addresses (UPDATE THESE)
IPAddress D1_IP(192, 168, 15, 201); // D1's IP
IPAddress D3_IP(192, 168, 15, 202); // D3's IP

// SOS and Motor Pins
const int SOS_BUTTON_PIN = 14;
const int MOTOR_PIN = 26;

// BLE Configuration
const char* DEVICE_NAME = "D4";

QMC5883LCompass compass;
WiFiClient clientD1;
WiFiClient clientD3;

void connectToServer(WiFiClient &client, IPAddress ip) {
  if (!client.connected()) {
    Serial.print("Connecting to ");
    Serial.print(ip);
    Serial.print("...");
    
    if (client.connect(ip, port)) {
      Serial.println("OK");
    } else {
      Serial.println("FAILED");
    }
  }
}

void sendData(WiFiClient &client, int heading) {
  if (client.connected()) {
    String packet = WiFi.macAddress() + "," + String(heading) + "\n";
    client.print(packet);
    Serial.println("Sent: " + packet);
  }
}

void alertSOS() {
  // Vibrate motor for SOS signal
  digitalWrite(MOTOR_PIN, HIGH);
  
  // Send SOS to both devices
  if (clientD1.connected()) clientD1.println("SOS:D4");
  if (clientD3.connected()) clientD3.println("SOS:D4");
  Serial.println("SOS signal sent");
  
  delay(1500);
  digitalWrite(MOTOR_PIN, LOW);
}

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22); // SDA, SCL
  
  // Initialize compass
  compass.init();
  Serial.println("Compass initialized");
  
  // Initialize I/O pins
  pinMode(SOS_BUTTON_PIN, INPUT_PULLUP);
  pinMode(MOTOR_PIN, OUTPUT);
  digitalWrite(MOTOR_PIN, LOW);
  
  // Connect to WiFi
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected! IP: " + WiFi.localIP().toString());
  
  // Initialize BLE Advertiser
  BLEDevice::init(DEVICE_NAME);
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService("1234");
  BLECharacteristic *pChar = pService->createCharacteristic(
    "ABCD", 
    BLECharacteristic::PROPERTY_READ
  );
  pChar->setValue("D4 Active");
  pService->start();
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->addServiceUUID(pService->getUUID());
  pAdvertising->start();
  Serial.println("BLE Advertising started");
}

void loop() {
  // Read compass heading
  compass.read();
  int selfHeading = compass.getAzimuth();
  Serial.print("Heading: ");
  Serial.print(selfHeading);
  Serial.println("°");
  
  // Connect to D1 and D3
  connectToServer(clientD1, D1_IP);
  connectToServer(clientD3, D3_IP);
  
  // Send heading data to both
  sendData(clientD1, selfHeading);
  sendData(clientD3, selfHeading);
  
  // Check SOS button
  if (digitalRead(SOS_BUTTON_PIN) == LOW) {
    Serial.println("SOS button pressed!");
    alertSOS();
  }
  
  delay(2000); // Main loop delay
}