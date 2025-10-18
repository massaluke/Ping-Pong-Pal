#include "BT.h"
#include "WiFiProj.h"
#include "Stepper.h"
#include "globals.h"
#include "FastLED.h"
#include <defs.h>
#include <ESP32Servo.h>
#include <cmath> // For fabs


CRGB leds[NUM_LEDS];        // Define the array of LEDs

// Default Packet
//ball number/frequency/Speed/Trajectory/Direction/Vertspin/Horizspin
int defaultMode[MODE_LENGTH] = {10, 10, 10, 0, 0, 0, 0};
int currentMode[MODE_LENGTH] = {10, 10, 10, 0, 0, 0, 0};

bool buttonDown = false;

int stepperPos = 0;
int actuatorPos = 0;
int ballsLeft = 0;
int interval = 500;
int motorSpeed = 0;
int spin = 0;

int successfulHits = 0;
int prevSuccessfulHits = 0;
int pastProcessedMessage = -1;
int processedMessage = 0;

// Bluetooth - Remote
bool BTsessionStarted = false;
bool BTsetStarted = false;
bool pauseSet = true;

// Remote settings
bool adaptive = false;
bool twoPlayer = false;

int shotTiming = -1000;

float leftSpeed = 0;
float rightSpeed = 0;
float topSpeed = 0;
float bottomSpeed = 0;

bool dataReturned = true;
bool dataReturned2P = true;

uint8_t broadcastAddr[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
Stepper myStepper(STEPS_PER_REV, STEPPER1, STEPPER2, STEPPER3, STEPPER4);

unsigned long previousMillis = 0;

//game
bool isPlayer1Turn = true;
unsigned long startTime = 0;
unsigned long player1TotalTime = 0;
unsigned long player2TotalTime = 0;
int player1Count = 0;
int player2Count = 0;

unsigned long turnInterval = 5000; // Base interval time
unsigned long lastTurnChange = 0;

unsigned long hitFlashStartTime = 0;
bool hitDetected = false;
bool pulseRainbow = true;
bool firstHitRegistered = false;
bool gameHit = false;
bool gameStartup = false;

void setupShoot() {
  // actuator setup
  if (currentMode[3] < 0) {
    digitalWrite(ACTUATOR_PH, HIGH);
    digitalWrite(ACTUATOR_EN, HIGH);
  } else if (currentMode[3] > 0) {
    digitalWrite(ACTUATOR_PH, LOW);
    digitalWrite(ACTUATOR_EN, HIGH);
  }
  
  // stepper stepup
  myStepper.step(currentMode[4] - stepperPos);
  stepperPos = currentMode[4];
  delay(200); // wait for stepper
  
  delay(abs(currentMode[3]*100));
  currentMode[3] = 0;
  digitalWrite(ACTUATOR_EN, LOW);
  
  if (currentMode[5] < 0) {
    topSpeed = fabs(currentMode[5]);
    bottomSpeed = 0;
  } else if (currentMode[5] > 0) {
    bottomSpeed = fabs(currentMode[5]);
    topSpeed = 0;
  } else {
    topSpeed = 1;
    bottomSpeed = 1;
  }

  if (currentMode[6] < 0) {
    rightSpeed = fabs(currentMode[6]);
    leftSpeed = 0;
  } else if (currentMode[6] > 0) {
    leftSpeed = fabs(currentMode[6]);
    rightSpeed = 0;
  } else {
    rightSpeed = 1;
    leftSpeed = 1;
  }

  leftSpeed *= (float)currentMode[2] / 100;
  rightSpeed *= (float)currentMode[2] / 100;
  topSpeed *= (float)currentMode[2] / 100;
  bottomSpeed *= (float)currentMode[2] / 100;

  ballsLeft = currentMode[0];
  dataReturned = false;
  interval = MILLIS_MIN/currentMode[1];

  stepperDetach();
}

bool canShoot() {
    int time = millis();
    if (time > (shotTiming + interval) && ballsLeft > 0 && BTsetStarted && !pauseSet) {
        ballsLeft--;
        shotTiming = millis();
        return true;
    } else if (ballsLeft == 0 && BTsetStarted) {
        SerialBT.println("Set Finished");
        BTsetStarted = false;
        return false;
    }
    return false;
}


void BMS() {
  if (analogRead(BATTERY_IND) < BATTERY_VAL) {

    digitalWrite(BATTERY_EN, LOW);
  } else {
    digitalWrite(BATTERY_EN, HIGH);
  }
}

void sendAverageTimes() {
  if (player1Count > 0) {
    SerialBT.print("Player 1 (blue) average time: ");
    SerialBT.print(player1TotalTime / player1Count);
    SerialBT.println(" ms");    
    Serial.print("Player 1 (blue) average time: ");
    Serial.print(player1TotalTime / player1Count);
    Serial.println(" ms");
  } else {
    SerialBT.println("Player 1 has no data.");
    Serial.println("Player 1 has no data.");
  }

  if (player2Count > 0) {
    SerialBT.print("Player 2 (yellow) average time: ");
    SerialBT.print(player2TotalTime / player2Count);
    SerialBT.println(" ms");    
    Serial.print("Player 2 (yellow) average time: ");
    Serial.print(player2TotalTime / player2Count);
    Serial.println(" ms");
  } else {
    SerialBT.println("Player 2 has no data.");
    Serial.println("Player 2 has no data.");
  }

  // Reset data
  player1TotalTime = 0;
  player2TotalTime = 0;
  player1Count = 0;
  player2Count = 0;
}

void stepperDetach() {
  digitalWrite(STEPPER1, LOW);
  digitalWrite(STEPPER2, LOW);
  digitalWrite(STEPPER3, LOW);
  digitalWrite(STEPPER4, LOW);
}

void initialiseHub() {
  // Disable all outputs
  analogWrite(MOTOR1, 0);
  analogWrite(MOTOR2, 0);
  analogWrite(MOTOR3, 0);
  analogWrite(MOTOR4, 0);
  digitalWrite(ACTUATOR_PH, LOW);
  digitalWrite(ACTUATOR_EN, LOW);
  digitalWrite(PWM_SERVO_PIN, LOW);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(STATUS_LED, LOW);
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(BATTERY_EN, LOW);

  // Initialise Batteries
  BMS();

  // Servo closed
  servoMove(false);

  // Stepper to centre
  // DO IT MANUALLY

  // LED flash
  pulsatingRed(800);
  // Turn off the LED strip
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CRGB::Black; // Set LED color to black (off)
  }
  FastLED.show(); // Display the LED colors

  // Default data packet
  txData.shoot = false;
  txData.twoP = false;
  txData.orientation = -1;
  txData.speed = -1;
  txData.ballDetected = -1;
  txData.hubMessage = 1;
  txData.remoteMessage = -13;

  rxData.shoot = false;
  rxData.orientation = -1;
  rxData.speed = -1;
  rxData.ballDetected = -1;
  rxData.hubMessage = -13;
  rxData.remoteMessage = -13;
  // Finished Sequence
  digitalWrite(STATUS_LED, HIGH);
  digitalWrite(4, HIGH);
  digitalWrite(17, HIGH);
}

Servo myservo;

void servoMove(bool status) {
  // Allow allocation of all timers
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  myservo.setPeriodHertz(50); // Standard 50hz servo
  pinMode(PWM_SERVO_PIN, OUTPUT);
  myservo.attach(PWM_SERVO_PIN, 500, 2400); // using SG90 servo min/max of 500us and 2400us
  if (!status) {
    myservo.write(SERVO_SHUT); // set the servo position according to the scaled value
    delay(SERVO_MOVE_TIME);
  } else {
    myservo.write(SERVO_OPEN); // set the servo position according to the scaled value
    delay(SERVO_OPEN_TIME);
    myservo.write(SERVO_SHUT); // set the servo position according to the scaled value
    delay(SERVO_MOVE_TIME);
  }
  myservo.detach();
  pinMode(PWM_SERVO_PIN, INPUT);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  if (esp_now_init() == ESP_OK) {
      esp_now_register_recv_cb(receiveCallback);
      esp_now_register_send_cb(sentCallback);
      Serial.println("ESP-NOW Init Success");
  } else {
      Serial.println("ESP-NOW Init Failed");
      delay(3000);
      ESP.restart();
  }

  SerialBT.begin("BLUEMAN");
  Serial.println("Bluetooth Started. Device name: BLUEMAN");

  // Initialise pins as outputs
  pinMode(MOTOR1, OUTPUT);
  pinMode(MOTOR2, OUTPUT);
  pinMode(MOTOR3, OUTPUT);
  pinMode(MOTOR4, OUTPUT);
  
  pinMode(ACTUATOR_PH, OUTPUT);
  pinMode(ACTUATOR_EN, OUTPUT);
  
  pinMode(STEPPER1, OUTPUT);
  pinMode(STEPPER2, OUTPUT);
  pinMode(STEPPER3, OUTPUT);
  pinMode(STEPPER4, OUTPUT);
  
  pinMode(PWM_SERVO_PIN, OUTPUT);
  
  pinMode(LED_PIN, OUTPUT);
  pinMode(STATUS_LED, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(17, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  
  pinMode(BATTERY_EN, OUTPUT);
  
  // Initialise pins as inputs
  pinMode(SWITCH, INPUT);
  pinMode(BATTERY_IND, INPUT);

  initialiseHub();

  myStepper.setSpeed(10);
  stepperDetach();

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
}

void loop() {
  BMS();
  // handle bluetooth inputs and process
  handleBluetoothData();

  if (successfulHits - prevSuccessfulHits == 1) {
    FastLED.showColor(CRGB::Green);
    Serial.println("Target hit!");
    SerialBT.println("Target hit!");
    prevSuccessfulHits = successfulHits;
    delay(500);
    FastLED.clear();
    FastLED.show();
  }

  if ((pastProcessedMessage != processedMessage) && (processedMessage != 0)) {
    pastProcessedMessage = processedMessage;
    processHexMessage(processedMessage);
    setupShoot();
    for (int i = 0; i < MODE_LENGTH; i++) {
      Serial.print(currentMode[i]);
      SerialBT.print(currentMode[i]);
      if (i < MODE_LENGTH - 1) {
        Serial.print(", ");
        SerialBT.print(", ");
      }
    }
    Serial.println();
    SerialBT.println();
  }
  // On board switch
  //if (!digitalRead(SWITCH)) {
    if (canShoot()) {
      txData.shoot = true;
      broadcast(txData, broadcastAddr);
      SerialBT.println("ball shooting...");
      digitalWrite(STATUS_LED, HIGH);
      digitalWrite(4, HIGH);
      digitalWrite(17, HIGH);
      servoMove(true);
      digitalWrite(STATUS_LED, LOW);
      digitalWrite(4, LOW);
      digitalWrite(17, LOW);
    }
  // } else {
  //   //motors off
  //   analogWrite(MOTOR1, 0);
  //   analogWrite(MOTOR2, 0);
  //   analogWrite(MOTOR3, 0);
  //   analogWrite(MOTOR4, 0);
  // }
  // if (millis() - previousMillis >= 4000) {
  //   previousMillis = millis();
  //   txData.shoot = true;
  //   broadcast(txData, broadcastAddr);
  //   SerialBT.println("ball shooting...");
  //   digitalWrite(STATUS_LED, HIGH);
  //   delay(200);
  //   digitalWrite(STATUS_LED, LOW);
  // }



  if (!ballsLeft && !dataReturned) {
    float rate = (float)successfulHits / (float)currentMode[0];
    Serial.print("Finished set. Your hit rate was: ");
    SerialBT.print("Finished set. Your hit rate was: ");
    Serial.print(rate * 100);
    SerialBT.print(rate * 100);
    Serial.println("%");
    SerialBT.println("%");

    if (adaptive) {
        Serial.println("Adaptive mode on");
        SerialBT.println("Adaptive mode on");

        if (rate > 0.75) {
            Serial.println("Success rate > 75%, increasing difficulty");
            SerialBT.println("Success rate > 75%, increasing difficulty");

            if (currentMode[2] < 90) {
                currentMode[2] = currentMode[2] + 10;
            }
            if (currentMode[1] < 27) {
                currentMode[1] = currentMode[1] + 3;
            }
        } else {
            Serial.println("Success rate < 75%, decreasing difficulty");
            SerialBT.println("Success rate < 75%, decreasing difficulty");

            if (currentMode[2] > 10) {
                currentMode[2] = currentMode[2] - 10;
            }
            if (currentMode[1] > 3) {
                currentMode[1] = currentMode[1] - 3;
            }
        }
    } else {
        Serial.println("Adaptive mode off");
        SerialBT.println("Adaptive mode off");
    }
    dataReturned = true;
    successfulHits = 0;
  }

if (twoPlayer) {
  if (gameStartup) {
    gameStartup = false;
    txData.twoP = true;
    broadcast(txData, broadcastAddr);
  }
    dataReturned2P = false;
    unsigned long currentMillis = millis();

    // Pulsating rainbow effect
    if (pulseRainbow) {
  static uint8_t hue = 0;
  static int pos = 0;
  static int direction = 1;

  // Fill all LEDs with a rainbow gradient
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(hue + (i * 256 / NUM_LEDS), 255, 100); // Base brightness for all LEDs
  }

  // Set the arrow formation with varying brightness
  leds[pos] = CHSV(hue + (pos * 256 / NUM_LEDS), 255, 255);          // Middle LED (super bright)
  if (pos > 0) {
    leds[pos - 1] = CHSV(hue + ((pos - 1) * 256 / NUM_LEDS), 255, 200); // Directly adjacent LED (bright)
  }
  if (pos < NUM_LEDS - 1) {
    leds[pos + 1] = CHSV(hue + ((pos + 1) * 256 / NUM_LEDS), 255, 200); // Directly adjacent LED (bright)
  }
  if (pos > 1) {
    leds[pos - 2] = CHSV(hue + ((pos - 2) * 256 / NUM_LEDS), 255, 150); // Second adjacent LED (medium bright)
  }
  if (pos < NUM_LEDS - 2) {
    leds[pos + 2] = CHSV(hue + ((pos + 2) * 256 / NUM_LEDS), 255, 150); // Second adjacent LED (medium bright)
  }

  FastLED.show();

  pos += direction;
  if (pos == NUM_LEDS - 1 || pos == 0) {
    direction = -direction;  // Reverse direction at the ends
  }
  hue += 1; // Adjust hue increment for smooth transition
  delay(40); // Adjust delay for smoother effect

      // // Check if hit during pulsing rainbow
      // if (gameHit) {
      //   // Double red flash
      gameHit = false;
      //   SerialBT.println("Wait for colour!");
      //   Serial.println("Wait for colour!");
      //   for (int j = 0; j < 2; j++) {
      //     FastLED.showColor(CRGB::Red);
      //     delay(250);
      //     FastLED.clear();
      //     FastLED.show();
      //     delay(250);
      //   }
      //   // Reset the timer
      //   lastTurnChange = currentMillis;
      //   turnInterval = random(3000, 7000);
      // }
    }

    // Randomly light up blue or yellow LED based on the interval
    if (!hitDetected && currentMillis - lastTurnChange >= turnInterval) {
      pulseRainbow = false;  // Stop pulsing
      if (random(2) == 0) {
        FastLED.showColor(CRGB::Blue);
        SerialBT.println("Player 1's (blue) turn!");
        Serial.println("Player 1's (blue) turn!");
        isPlayer1Turn = true;
      } else {
        FastLED.showColor(CRGB::Yellow);
        SerialBT.println("Player 2's (yellow) turn!");
        Serial.println("Player 2's (yellow) turn!");
        isPlayer1Turn = false;
      }
      startTime = millis();
      lastTurnChange = currentMillis; // Reset the change timer to ensure LED stays visible
    }

    // Check if target was hit
    if (gameHit && !firstHitRegistered) {
      gameHit = false;
      firstHitRegistered = true;
      hitDetected = true;
      hitFlashStartTime = currentMillis;
      FastLED.showColor(CRGB::Green);
      Serial.println("Target hit!");
      SerialBT.println("Target hit!");
      unsigned long responseTime = millis() - startTime;
      if (isPlayer1Turn) {
        player1TotalTime += responseTime;
        player1Count++;
        Serial.print("Player 1 hit target in ");
        Serial.print(responseTime);
        Serial.println(" ms");
        SerialBT.print("Player 1 hit target in ");
        SerialBT.print(responseTime);
        SerialBT.println(" ms");
      } else {
        player2TotalTime += responseTime;
        player2Count++;
        Serial.print("Player 2 hit target in ");
        Serial.print(responseTime);
        Serial.println(" ms");
        SerialBT.print("Player 2 hit target in ");
        SerialBT.print(responseTime);
        SerialBT.println(" ms");
      }

      // Reset turn change timer for next turn
      lastTurnChange = currentMillis; 
      turnInterval = random(3000, 7000); // Randomize interval for next turn
    }

    // Return to rainbow effect after 500ms flash
    if (hitDetected && currentMillis - hitFlashStartTime >= 500) {
      hitDetected = false;
      pulseRainbow = true;
      firstHitRegistered = false;
      Serial.println("Returning to rainbow effect...");
    }
  }

  if (!twoPlayer && !dataReturned2P) {
    sendAverageTimes();
    dataReturned2P = true;
    txData.twoP = false;
    broadcast(txData, broadcastAddr);
    FastLED.clear();
    FastLED.show();
  }
}
