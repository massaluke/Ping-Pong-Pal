#include "WiFiProj.h"
#include "BT.h"
#include <defs.h>

// Define LED and pushbutton pins
#define TARGET_LED 15
#define STATUS_LED 2

#define PZE 33
#define BATTERY_VAL_T 2300
#define BATT_STAT 2

// Define the retry delay in milliseconds
#define RETRY_DELAY 20

// Send a message to a specific MAC address
//uint8_t specificAddr[] = {0xd0, 0xef, 0x76, 0x69, 0x06, 0xe0}; // Specific MAC address
//uint8_t specificAddr[] = {0xd0, 0xef, 0x76, 0x11, 0x3f, 0x84}; // PCB
uint8_t specificAddr[] = {0xd0, 0xef, 0x76, 0x11, 0x40, 0x24}; // NEW PCB


// Define LED and pushbutton state booleans
bool ledOn = false;
int ballTime = 0;
int ballShot = 0;
int successfulHits = -1;
int processedMessage = -1;

unsigned long lastLowTime = 0;
unsigned long lastHighTime = 0;

// supressions :0
bool gameHit = false;

void BMS() {
  int batteryValue = analogRead(BATTERY_IND);
  if (batteryValue < BATTERY_VAL_T) {
    if (lastLowTime == 0) {
      lastLowTime = millis();
    } else if (millis() - lastLowTime > 1000) {
      digitalWrite(BATT_STAT, HIGH);
      lastHighTime = 0; 
    }
  } else {
    lastLowTime = 0;
    if (lastHighTime == 0) {
      lastHighTime = millis();
    } else if (millis() - lastHighTime > 1000) {
      digitalWrite(BATT_STAT, LOW);
      lastLowTime = 0; 
    }
  }
}

void setup() {
  // Set up Serial Monitor
  Serial.begin(115200);
  delay(1000);

  // Set ESP32 in STA mode to begin with
  WiFi.mode(WIFI_STA);
  Serial.println("ESP-NOW Broadcast");

  // Print MAC address
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());

  // Disconnect from WiFi
  WiFi.disconnect();

  // Initialize ESP-NOW
  if (esp_now_init() == ESP_OK) {
    Serial.println("ESP-NOW Init Success");
    esp_now_register_recv_cb(receiveCallback);
    esp_now_register_send_cb(sentCallback);
  } else {
    Serial.println("ESP-NOW Init Failed");
    delay(3000);
    ESP.restart();
  }

  // Pushbutton uses built-in pullup resistor, use INPUT_PULLUP
  pinMode(PZE, INPUT);

  // LED Output
  pinMode(TARGET_LED, OUTPUT);
  pinMode(STATUS_LED, OUTPUT);
  pinMode(STATUS_LED, HIGH);

  txData.shoot = false;
  txData.orientation = 10;
  txData.speed = 15;
  txData.ballDetected = -1;
  txData.hubMessage = 20;

  rxData.shoot = false;
  rxData.orientation = -1;
  rxData.speed = -1;
  rxData.ballDetected = -1;
  rxData.hubMessage = -13;
  rxData.remoteMessage = -13;
  delay(2000);
}

void loop() {
  BMS();
  if (rxData.shoot) {
      Serial.println("Hub message received");
      digitalWrite(TARGET_LED, HIGH);
      rxData.shoot = false;
      ballShot = true;
      ballTime = millis();
  }

  int time = millis();
  Serial.println(analogRead(PZE));
  
  if (ballShot && (time - ballTime < 1500)) {
      Serial.println("WAITING FOR HIT");
      if (analogRead(PZE) > 0) {
          digitalWrite(TARGET_LED, LOW);
          txData.ballDetected = 1;
          ballShot = false;
          broadcast(txData, specificAddr);
          Serial.println("Broadcasting ballDetected = 1");
      }
  } else if (ballShot) {
      txData.ballDetected = 0;
      ballShot = false;
      broadcast(txData, specificAddr);
      digitalWrite(TARGET_LED, LOW);
  }

  if (rxData.twoP) {
    if (analogRead(PZE) > 0) {
        txData.ballDetected = 2;
        broadcast(txData, specificAddr);
    }
  }
}
