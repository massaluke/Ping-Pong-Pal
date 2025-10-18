#include "RunSettings.h"

// External global variables, assuming they are defined elsewhere
extern BluetoothSerial SerialBT;
extern bool BTsessionStarted;
extern bool BTsetStarted;
extern int BTcom;
extern int successfulHits;
extern bool adaptive;
extern bool twoPlayer;
extern bool gameStartup;

extern int defaultMode[MODE_LENGTH];
extern int currentMode[MODE_LENGTH];
extern CRGB leds[]; // Define the array of LEDs

extern bool pauseSet;

extern float leftSpeed;
extern float rightSpeed;
extern float topSpeed;
extern float bottomSpeed;

//ball number/frequency/Speed/Trajectory/Direction/Vertspin/Horizspin

// increasing in difficulty
int drills[9][MODE_LENGTH] = {
    {3, 20, 100, 0, 0, 0, 70}, // default test drill
    {5, 20, 100, 0, 0, 0, 75}, // longer default
    {10, 3, 100, 0, 0, 0, 80}, // actual drill
    {10, 10, 100, 0, 0, -50, 0}, // fast, direction and vertspin
    {10, 20, 30, 0, 0, 100, 0}, // slow, direction and vertspin
    {10, 30, 100, 0, 0, 0, 25}, // fast, direction and horizspin
    {10, 30, 30, 0, 0, 0, -75}, // slow, direction and horizspin
    {10, 30, 100, 30, -50, 0, 0}, // trajectory and direction
    {10, 30, 100, -30, 50, 0, 0} // trajectory and direction
};

void copyArray(int drillNum) {
    for (int i = 0; i < MODE_LENGTH; ++i) {
        currentMode[i] = drills[drillNum - 1][i];
    }
}

void processHexMessage(int processedMessage)
{
  switch (processedMessage)
  {
  case 0x51:
    if (BTsessionStarted)
    {
      SerialBT.println("Session already started");
    }
    else
    {
      BTsessionStarted = true;
      SerialBT.println("Starting session...");
      SerialBT.println("Default mode loaded:");
      setDefaultMode();
      printCurrentMode();
    }
    break;
  case 0x52:
    if (!BTsetStarted)
    {
      SerialBT.println("No set running");
    }
    else
    {
      BTsetStarted = false;
      pauseSet = true;
      SerialBT.println("Stopping set...");
      stop();
    }
    break;
  case 0x01:
    if (BTsetStarted)
    {
      SerialBT.println("Set already started");
    }
    else if (!BTsessionStarted)
    {
      SerialBT.println("No session started");
    }
    else
    {
      BTsetStarted = true;
      pauseSet = false;
      SerialBT.println("Starting set...");
      run();
      // Display a pulsating green effect on the LED strip for 5 seconds
      pulsatingGreen(2000);
      // Turn off the LED strip
      for (int i = 0; i < NUM_LEDS; i++)
      {
        leds[i] = CRGB::Black; // Set LED color to black (off)
      }
      FastLED.show(); // Display the LED colors
    }
    break;
  case 0x02:
    if (BTsessionStarted)
    {
      SerialBT.println("Powering off...");
      BTsessionStarted = false;
      BTsetStarted = false;
      pauseSet = true;
      stop();
      // Display a pulsating green effect on the LED strip for 5 seconds
      pulsatingRed(2000); 
      // Turn off the LED strip
      for (int i = 0; i < NUM_LEDS; i++)
      {
        leds[i] = CRGB::Black; // Set LED color to black (off)
      }
      FastLED.show(); // Display the LED colors
    }
    else
    {
      SerialBT.println("No session to power off.");
    }
    break;
  case 0x03:
    if (!pauseSet)
    {
      SerialBT.println("Pausing...");
      pauseSet = true;
      stop();
    }
    else
    {
      SerialBT.println("No set to pause.");
    }
    break;
  case 0x04:
    if (BTsetStarted && BTsessionStarted && pauseSet)
    {
      SerialBT.println("Resuming...");
      pauseSet = false;
      run();
    }
    else
    {
      SerialBT.println("No set to resume or session not started.");
    }
    break;
  case 0x05:
    if (!adaptive) {
      SerialBT.println("Adaptive on");
      adaptive = true;
    } else {
      adaptive = false;
      SerialBT.println("Adaptive off");
    }
    break;
  case 0x07:
      twoPlayer = true;
      gameStartup = true;
      SerialBT.println("Two Player on");
      Serial.println("Two Player on");
    break;
  case 0x06:
      twoPlayer = false;
      Serial.println("Two Player off");
      SerialBT.println("Two Player off");
    break;
  // Add cases for customization settings
  case 0x0A:
  case 0x0B:
  case 0x0C:
  case 0x0D:
  case 0x0E:
  case 0x0F:
  case 0x10:
  case 0x11:
  case 0x12:
  case 0x13:
  // speed as a percent from 10 - 100%
    currentMode[2] = (processedMessage - 0x09) * 10;
    SerialBT.println("Setting Speed: ");
    SerialBT.print((processedMessage - 0x09) * 10);
    SerialBT.println("%");
    break;
  case 0x14:
  case 0x15:
  case 0x16:
  case 0x17:
  case 0x18:
  case 0x19:
  case 0x1A:
  case 0x1B:
  case 0x1C:
  // trajectory from -60° to 60°
    currentMode[3] = (processedMessage - 0x18) * 15;
    SerialBT.println("Setting Trajectory: ");
    SerialBT.print((processedMessage - 0x18) * 15);
    SerialBT.println("°");
    break;
  case 0x1E:
  case 0x1F:
  case 0x20:
  case 0x21:
  case 0x22:
    currentMode[5] = (processedMessage - 0x20) * 50;
    SerialBT.println("Setting Vertical Spin: ");
    if ((processedMessage - 0x20) < 0)
    {
      SerialBT.print("top: ");
      SerialBT.print((processedMessage - 0x20) * -50);
    }
    else
    {
      SerialBT.print("back: ");
      SerialBT.print((processedMessage - 0x20) * 50);
    }
    SerialBT.println("%");
    break;
  case 0x28:
  case 0x29:
  case 0x2A:
  case 0x2B:
  case 0x2C:
  case 0x2D:
  case 0x2E:
  case 0x2F:
  case 0x30:
  case 0x31:
    currentMode[6] = (processedMessage - 0x2C) * 25;
    SerialBT.println("Setting Horizontal Spin: ");
    if ((processedMessage - 0x2C) < 0)
    {
      SerialBT.print("left: ");
      SerialBT.print((processedMessage - 0x2C) * -25 );
    }
    else
    {
      SerialBT.print("right: ");
      SerialBT.print((processedMessage - 0x2C) * 25);
    }
    SerialBT.println("%");
    break;
  case 0x32:
  case 0x33:
  case 0x34:
  case 0x35:
  case 0x36:
  case 0x37:
  case 0x38:
  case 0x39:
  case 0x3A:
  case 0x3B:
    // direction from -60° to 60°
    currentMode[4] = (processedMessage - 0x36) * 15;
    SerialBT.println("Setting Direction: ");
    SerialBT.print((processedMessage - 0x36) * 15);
    SerialBT.println("°");
    break;
  case 0x3C:
  case 0x3D:
  case 0x3E:
  case 0x3F:
  case 0x40:
  case 0x41:
  case 0x42:
  case 0x43:
  case 0x44:
  case 0x45:
    // balls per minute
    currentMode[1] = (processedMessage - 0x3B) * 3;
    SerialBT.println("Setting Balls per Minute: ");
    SerialBT.print((processedMessage - 0x3C) * 3);
    SerialBT.println("bpm");
    break;
  case 0x46:
    SerialBT.println("Drill 1");
    copyArray(1);
    break;
  case 0x47:
    SerialBT.println("Drill 2");
    copyArray(2);
    break;
  case 0x48:
    SerialBT.println("Drill 3");
    copyArray(3);
    break;
  case 0x49:
    SerialBT.println("Drill 4");
    copyArray(4);
    break;
  case 0x4A:
    SerialBT.println("Drill 5");
    copyArray(5);
    break;
  case 0x4B:
    SerialBT.println("Drill 6");
    copyArray(6);
    break;
  case 0x4C:
    SerialBT.println("Drill 7");
    copyArray(7);
    break;
  case 0x4D:
    SerialBT.println("Drill 8");
    copyArray(8);
    break;
  case 0x4E:
    SerialBT.println("Drill 9");
    copyArray(9);
    break;
  case 0x4F:
    SerialBT.println("Drill 10");
    copyArray(10);
    break;
  case 0x50:
    SerialBT.println("Low Battery, peripherals off");
    Serial.println("Low Battery, peripherals off");
    break;
  default:
    if (processedMessage >= 0x60)
    {
      currentMode[0] = (processedMessage - 0x59);
      SerialBT.println("Setting Total Balls: ");
      SerialBT.println(processedMessage - 0x60);
    }
    else
    {
      SerialBT.println("Unknown command.");
      SerialBT.println(processedMessage);
    }
    break;
  }
}

void pulsatingGreen(unsigned long duration)
{
  // Define variables for pulsating effect
  int brightnessMin = 0;   // Minimum brightness (0-255)
  int brightnessMax = 200; // Maximum brightness (0-255)
  int brightnessStep = 5;  // Step size for brightness change
  int delayTime = 5;      // Delay time between brightness changes

  unsigned long startTime = millis(); // Get the current time

  // Perform the pulsating effect loop for the specified duration
  while (millis() - startTime < duration)
  {
    // Increase brightness from min to max
    for (int brightness = brightnessMin; brightness <= brightnessMax; brightness += brightnessStep)
    {
      // Set all LEDs to green color with the current brightness level
      for (int i = 0; i < NUM_LEDS; i++)
      {
        leds[i] = CRGB(0, brightness, 0); // Green color with varying brightness
      }
      FastLED.show();   // Update the LEDs
      delay(delayTime); // Delay before next brightness change
    }

    // Decrease brightness from max to min
    for (int brightness = brightnessMax; brightness >= brightnessMin; brightness -= brightnessStep)
    {
      // Set all LEDs to green color with the current brightness level
      for (int i = 0; i < NUM_LEDS; i++)
      {
        leds[i] = CRGB(0, brightness, 0); // Green color with varying brightness
      }
      FastLED.show();   // Update the LEDs
      delay(delayTime); // Delay before next brightness change
    }
  }
}

void pulsatingRed(unsigned long duration)
{
  // Define variables for pulsating effect
  int brightnessMin = 0;   // Minimum brightness (0-255)
  int brightnessMax = 200; // Maximum brightness (0-255)
  int brightnessStep = 5;  // Step size for brightness change
  int delayTime = 20;      // Delay time between brightness changes

  unsigned long startTime = millis(); // Get the current time

  // Perform the pulsating effect loop for the specified duration
  while (millis() - startTime < duration)
  {
    // Increase brightness from min to max
    for (int brightness = brightnessMin; brightness <= brightnessMax; brightness += brightnessStep)
    {
      // Set all LEDs to red color with the current brightness level
      for (int i = 0; i < NUM_LEDS; i++)
      {
        leds[i] = CRGB(brightness, 0, 0); // Red color with varying brightness
      }
      FastLED.show();   // Update the LEDs
      delay(delayTime); // Delay before next brightness change
    }

    // Decrease brightness from max to min
    for (int brightness = brightnessMax; brightness >= brightnessMin; brightness -= brightnessStep)
    {
      // Set all LEDs to red color with the current brightness level
      for (int i = 0; i < NUM_LEDS; i++)
      {
        leds[i] = CRGB(brightness, 0, 0); // Red color with varying brightness
      }
      FastLED.show();   // Update the LEDs
      delay(delayTime); // Delay before next brightness change
    }
  }
}

void run() {
  Serial.println("RUNNING");
  analogWrite(MOTOR1, 255*leftSpeed);
  analogWrite(MOTOR2, 255*rightSpeed);
  analogWrite(MOTOR3, 255*topSpeed);
  analogWrite(MOTOR4, 255*bottomSpeed);
}

void stop() {
  Serial.println("STOPPING");
  analogWrite(MOTOR1, 0);
  analogWrite(MOTOR2, 0);
  analogWrite(MOTOR3, 0);
  analogWrite(MOTOR4, 0);
}