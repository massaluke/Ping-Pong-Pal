#ifndef WIFIPROJ_H
#define WIFIPROJ_H

#include <WiFi.h>
#include <esp_now.h>
#include <Arduino.h>
#include "globals.h"


// Define the retry delay in milliseconds
#define RETRY_DELAY 20

struct bigData {
  bool shoot;
  bool twoP;
  int orientation;
  int speed;
  int ballDetected;
  int hubMessage;
  int remoteMessage;
};


// Declare external instances of the structure
extern bigData txData;
extern bigData rxData;

extern bool gameHit;

void formatMacAddress(const uint8_t *macAddr, char *buffer, int maxLength);
void receiveCallback(const esp_now_recv_info *info, const uint8_t *data, int dataLen);
void sentCallback(const uint8_t *macAddr, esp_now_send_status_t status);
void broadcast(const bigData& data, const uint8_t *specificAddr);

#endif // WIFE_H
