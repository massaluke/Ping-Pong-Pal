#include <HardwareSerial.h>
#include "WiFiProj.h"
#include <defs.h>

// Define LED and pushbutton pins
#define HIT_LED 5

// Define the retry delay in milliseconds
#define RETRY_DELAY 20

#define RX2 18
#define TX2 17

#define BATTERY_EN_R 32
#define BATTERY_VAL_R 1900
#define BATT_STAT 13

// Send a message to a specific MAC address // change in wife.cpp
//uint8_t specificAddr[] = {0xd0, 0xef, 0x76, 0x69, 0x06, 0xe0}; // TAPE DEV BOARD
//uint8_t specificAddr[] = {0xd0, 0xef, 0x76, 0x11, 0x3f, 0x84}; // PCB
uint8_t specificAddr[] = {0xd0, 0xef, 0x76, 0x11, 0x40, 0x24}; // NEW PCB

// Define LED and pushbutton state booleans
bool ledOn = false;
int ballTime = 0;
int ballShot = 0;
int successfulHits = -1;
int processedMessage = -1;
int incomingByte = 0;

int EN = 32;
unsigned long lastLowTime = 0;
unsigned long lastHighTime = 0;
bool gameHit = false;


// HardwareSerial --> better performance
HardwareSerial serialInput(1);

void BMS() {
  int batteryValue = analogRead(BATTERY_IND);
  Serial.println(batteryValue);
  if (batteryValue < BATTERY_VAL_R) {
    if (lastLowTime == 0) {
      lastLowTime = millis();
    } else if (millis() - lastLowTime > 1000) {
      digitalWrite(BATT_STAT, HIGH);
      digitalWrite(BATTERY_EN_R, LOW);
      lastHighTime = 0; 
    }
  } else {
    lastLowTime = 0;
    if (lastHighTime == 0) {
      lastHighTime = millis();
    } else if (millis() - lastHighTime > 1000) {
      digitalWrite(BATTERY_EN_R, HIGH);
      digitalWrite(BATT_STAT, LOW);
      lastLowTime = 0; 
    }
  }
}
void setup() {
  // Initialize serial communication at a baud rate of 9600
  Serial.begin(9600);
  // Initialize the second hardware serial port with a baud rate matching Nextion display
  serialInput.begin(9600, SERIAL_8N1, RX2, TX2); // 16 -> RX2, 17 -> TX2
  Serial.println("Serial communication with Nextion display started");
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

  // LED Output
  pinMode(HIT_LED, OUTPUT);
  pinMode(BATT_STAT, OUTPUT);

  // enable buck
  pinMode(EN, OUTPUT);
  digitalWrite(EN, HIGH);
  txData.shoot = 5;
  txData.orientation = 10;
  txData.speed = 15;
  txData.ballDetected = -1;
  txData.hubMessage = 20;
  txData.remoteMessage = -1;

  rxData.shoot = false;
  rxData.orientation = -1;
  rxData.speed = -1;
  rxData.ballDetected = -1;
  rxData.hubMessage = -13;
  txData.remoteMessage = -1;
}

void loop() {
  BMS();
  // Check if data is available to read from Nextion display
  if (serialInput.available() > 0) {
    // Read the incoming byte
    incomingByte = serialInput.read();
    
    // Print the received byte as an integer value
    Serial.print("Received from Nextion: ");
    Serial.println(incomingByte);
    // Print a new line
    Serial.println();
    if (incomingByte != 0) {
      digitalWrite(HIT_LED, HIGH);
      txData.remoteMessage = incomingByte;
      ballShot = false;
      broadcast(txData, specificAddr);
      incomingByte = 0;
    }
  }
  // Delay for serial monitor
  delay(100);
  digitalWrite(HIT_LED, LOW);
}