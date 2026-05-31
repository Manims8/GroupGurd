/*
 * =================================================================
 * D2 - PERIPHERAL DEVICE (NO DISPLAY) - WI-FI VERSION (UNIFIED)
 * =================================================================
 *
 * Role:
 * 1. Acts as a TCP client to send data to D1.
 * 2. Hosts a TCP server to listen for warnings from D1.
 * 3. Reads compass and sends heading, SOS status, and Wi-Fi RSSI.
 * 4. Activates vibration motor and LED on warning or SOS trigger.
 *
 * HARDWARE: ESP32, QMC5883L, Button, Vibration Motor, LED
 *
 */

// LIBRARIES
#include <WiFi.h>
#include <Wire.h>
#include <QMC5883LCompass.h>

// =================== NETWORK CONFIGURATION ===================
const char* ssid = "GroupGuard";
const char* password = "Groupguard@0826";
IPAddress D1_SERVER_IP;
const uint16_t D1_SERVER_PORT = 12345;
const uint16_t MY_SERVER_PORT = 54321;
WiFiServer my_server(MY_SERVER_PORT);

// =================== HARDWARE & DEVICE CONFIGURATION ===================
#define I2C_SDA 21
#define I2C_SCL 22
#define BUTTON_PIN 15
#define VIBRATION_MOTOR_PIN 5
#define LED_PIN 25 // Optional LED (Active-High)
#define DEVICE_NAME "D2"

QMC5883LCompass compass;

// =================== GLOBAL VARIABLES & STATE ===================
int dir[10] = {0,0,0,0,0,0,0,0,0,0};
int hed;
int myHeading = 0;
bool sosActive = false;
volatile bool warningReceived = false;

unsigned long lastSendTime = 0;
const unsigned long sendInterval = 1000;

// =================== UNIFIED ALERT STATE MACHINE ===================
bool alert_active = false;
unsigned long alert_start_millis = 0;
unsigned long alert_duration = 0;
unsigned long last_vibe_toggle = 0;
unsigned long last_led_toggle = 0;
int vibe_pulse_on_time = 0;
int vibe_pulse_off_time = 0;
int led_pulse_on_time = 0;
int led_pulse_off_time = 0;
bool vibe_pulse_state = false;
bool led_pulse_state = false;

void triggerAlert(unsigned long duration, int vibeOn, int vibeOff, int ledOn, int ledOff) {
    alert_active = true;
    alert_start_millis = millis();
    alert_duration = duration;
    vibe_pulse_on_time = vibeOn;
    vibe_pulse_off_time = vibeOff;
    led_pulse_on_time = ledOn;
    led_pulse_off_time = ledOff;
    
    // Initial vibration state
    if (vibeOn > 0) {
        vibe_pulse_state = true;
        digitalWrite(VIBRATION_MOTOR_PIN, HIGH);
    } else {
        vibe_pulse_state = false;
        digitalWrite(VIBRATION_MOTOR_PIN, LOW);
    }
    
    // Initial LED state
    if (ledOn > 0) {
        led_pulse_state = true;
        digitalWrite(LED_PIN, HIGH);
    } else {
        led_pulse_state = false;
        digitalWrite(LED_PIN, LOW);
    }
    
    last_vibe_toggle = millis();
    last_led_toggle = millis();
}

void handleAlerts() {
    if (!alert_active) {
        digitalWrite(VIBRATION_MOTOR_PIN, LOW);
        digitalWrite(LED_PIN, LOW);
        return;
    }
    
    // Check if alert duration has expired
    if (millis() - alert_start_millis > alert_duration) {
        alert_active = false;
        digitalWrite(VIBRATION_MOTOR_PIN, LOW);
        digitalWrite(LED_PIN, LOW);
        return;
    }
    
    // Handle non-blocking vibration pulsing
    if (vibe_pulse_on_time > 0 && vibe_pulse_off_time > 0) {
        unsigned long current_vibe_interval = vibe_pulse_state ? vibe_pulse_on_time : vibe_pulse_off_time;
        if (millis() - last_vibe_toggle >= current_vibe_interval) {
            vibe_pulse_state = !vibe_pulse_state;
            digitalWrite(VIBRATION_MOTOR_PIN, vibe_pulse_state ? HIGH : LOW);
            last_vibe_toggle = millis();
        }
    } else if (vibe_pulse_on_time == 0) {
        digitalWrite(VIBRATION_MOTOR_PIN, LOW);
    } else {
        digitalWrite(VIBRATION_MOTOR_PIN, HIGH);
    }
    
    // Handle non-blocking LED pulsing
    if (led_pulse_on_time > 0 && led_pulse_off_time > 0) {
        unsigned long current_led_interval = led_pulse_state ? led_pulse_on_time : led_pulse_off_time;
        if (millis() - last_led_toggle >= current_led_interval) {
            led_pulse_state = !led_pulse_state;
            digitalWrite(LED_PIN, led_pulse_state ? HIGH : LOW);
            last_led_toggle = millis();
        }
    } else if (led_pulse_on_time == 0) {
        digitalWrite(LED_PIN, LOW);
    } else {
        digitalWrite(LED_PIN, HIGH);
    }
}

// =================== FUNCTION PROTOTYPES ===================
void connectToWiFi();
void handleIncomingWarnings();
void sendDataToMaster();
int readCompassHeading();

// =================== SETUP ===================
void setup() {
  Serial.begin(115200);
  Wire.begin(I2C_SDA, I2C_SCL);

  // Init hardware
  compass.init();
  pinMode(BUTTON_PIN, INPUT_PULLDOWN);
  pinMode(VIBRATION_MOTOR_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(VIBRATION_MOTOR_PIN, LOW);
  digitalWrite(LED_PIN, LOW);

  // Set D1's IP to SoftAP address
  D1_SERVER_IP.fromString("192.168.4.1");

  // Connect to Wi-Fi and start server
  connectToWiFi();
  my_server.begin();
  Serial.println("D2: Ready");
}

// =================== MAIN LOOP ===================
void loop() {
  // 1. Check for incoming warnings from D1
  handleIncomingWarnings();

  // 2. Handle warning received (Pulses 300ms ON / 100ms OFF for 3s, LED solid)
  if (warningReceived) {
    Serial.println("[D2] Warning received!");
    triggerAlert(3000, 300, 100, 150, 150); 
    warningReceived = false; 
  }
  
  // 3. Process non-blocking alerts
  handleAlerts();

  // 4. Periodically send data to D1
  if (millis() - lastSendTime > sendInterval) {
    // Check SOS button status
    sosActive = (digitalRead(BUTTON_PIN) == HIGH);
    if (sosActive) {
      Serial.println("[D2] SOS ACTIVE!");
      triggerAlert(2000, 200, 200, 2000, 0); 
    }

    myHeading = readCompassHeading();

    // Send data packet
    sendDataToMaster();
    lastSendTime = millis();
  }
}

// =================== NETWORK FUNCTIONS ===================
void connectToWiFi() {
  Serial.print("Connecting to "); Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected.");
  Serial.print("D2 IP address: "); Serial.println(WiFi.localIP());
}

void handleIncomingWarnings() {
  WiFiClient client = my_server.available();
  if (client) {
    String command = client.readStringUntil('\n');
    client.stop();
    command.trim();
    if (command == "WARN") {
      warningReceived = true;
    }
  }
}

void sendDataToMaster() {
  WiFiClient client;
  if (!client.connect(D1_SERVER_IP, D1_SERVER_PORT)) {
    Serial.println("Connection to D1 master failed!");
    return;
  }

  // Create data packet: "NAME,HEADING,SOS,RSSI"
  long rssi = WiFi.RSSI();
  char dataPacket[64];
  snprintf(dataPacket, sizeof(dataPacket), "%s,%d,%d,%ld", DEVICE_NAME, myHeading, sosActive ? 1 : 0, rssi);

  client.println(dataPacket);
  client.stop();
  Serial.print("Sent data: "); Serial.println(dataPacket);
}

int readCompassHeading() {
  hed = 0;
  compass.read();
  for (int i = 0; i <= 9; ++i) {
    dir[i] = compass.getAzimuth();
    hed += dir[i];
  }
  return hed / 10;
}
