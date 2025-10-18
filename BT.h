#ifndef BT_H
#define BT_H

#include <BluetoothSerial.h>
#include <Arduino.h>
#include <FastLED.h>

// Define LED and pushbutton pins
#define STATUS_LED 2
#define STATUS_BUTTON 22
#define LED_PIN     5       // Data pin connected to the LED strip
#define NUM_LEDS    62      // Number of LEDs in the strip
#define BRIGHTNESS  64      // Brightness level (0-255)
#define LED_TYPE    WS2812B // Type of the LED strip
#define COLOR_ORDER GRB      // Color order of the LED strip

// Function prototype for processing hex messages
void processHexMessage(int processedMessage);
void pulsatingGreen(unsigned long duration);
void pulsatingRed(unsigned long duration);
void run();
void stop();
#endif // BIG_H