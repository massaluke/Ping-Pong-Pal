#ifndef DEFS_H
#define DEFS_H

// pins
#define MOTOR1 25
#define MOTOR2 26
#define MOTOR3 27
#define MOTOR4 14


#define ACTUATOR_PH 15
#define ACTUATOR_EN 13

#define STEPPER1 19
#define STEPPER2 21
#define STEPPER3 22
#define STEPPER4 23

#define PWM_SERVO_PIN 18

#define LED_PIN 5

#define SWITCH 34
  
#define STATUS_LED 2
#define LED1 4
#define LED2 17

#define BATTERY_IND 35
#define BATTERY_EN 33
#define BATTERY_VAL 2000

// max min params
#define STEPPER_MAX 100
#define STEPPER_MIN 0
#define ACTUATOR_MAX 180
#define ACTUATOR_MIN 0

#define ADC_MAX 4095

#define MILLIS_MIN 60000
#define PERCENT 100

// LED STRIP
#define NUM_LEDS    24      // Number of LEDs in the strip
#define BRIGHTNESS  64      // Brightness level (0-255)
#define LED_TYPE    WS2812B // Type of the LED strip
#define COLOR_ORDER GRB      // Color order of the LED strip

#define MODE_LENGTH 7

// Actuator
#define ACTUATOR_RESET_TIME 7000

// Bluetooth
#define MAX_SET_NUMBER 20

#define BT_QUERY 1
#define BT_DATA_RESPONSE 2
#define BT_MODE_RESPONSE 3

// Servo
#define SERVO_OPEN 120
#define SERVO_SHUT 30
#define SERVO_OPEN_TIME 150
#define SERVO_MOVE_TIME 600

// Stepper
#define STEPS_PER_REV 200


#endif