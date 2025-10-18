#include "WiFiProj.h"

// Define the instances of the structures
bigData txData;
bigData rxData;
long lastChange = 0;

void formatMacAddress(const uint8_t *macAddr, char *buffer, int maxLength) {
  snprintf(buffer, maxLength, "%02x:%02x:%02x:%02x:%02x:%02x", macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
}

void receiveCallback(const esp_now_recv_info *info, const uint8_t *data, int dataLen) {
  const uint8_t *macAddr = info->src_addr;

  
  if (dataLen != sizeof(rxData)) {
    Serial.println("Received data length mismatch");
    return;
  }

  memcpy(&rxData, data, sizeof(rxData));

  char macStr[18];
  formatMacAddress(macAddr, macStr, 18);

  Serial.printf("Received message from: %s -> Remote: %d, Ball Detection: %d\n", macStr, rxData.remoteMessage, rxData.ballDetected);
  if (rxData.ballDetected == 1) {
    Serial.println("SUCCESSFUL HIT!");
    successfulHits++;
  } else if (rxData.ballDetected == 2) {
    if ((millis() - lastChange) > 100) {
      lastChange = millis();
      gameHit = true;
    }
  }
  if ((rxData.remoteMessage != -1) && (rxData.remoteMessage != 0)) {
    processedMessage = rxData.remoteMessage;
  }
}

void sentCallback(const uint8_t *macAddr, esp_now_send_status_t status) {
  char macStr[18];
  formatMacAddress(macAddr, macStr, 18);
  Serial.print("Last Packet Sent to: ");
  Serial.println(macStr);
  Serial.print("Last Packet Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");

  if (status == ESP_NOW_SEND_FAIL) {
    Serial.println("Retrying transmission in 20ms...");
    delay(RETRY_DELAY);
    //uint8_t specAddr[] = {0xd0, 0xef, 0x76, 0x11, 0x3f, 0x84}; old guy
    //uint8_t specAddr[] = {0xd0, 0xef, 0x76, 0x69, 0x06, 0xe0}; big guy
    uint8_t specAddr[] = {0xd0, 0xef, 0x76, 0x11, 0x40, 0x24}; // NEW PCB

    broadcast(txData, specAddr);
  }
}

void broadcast(const bigData &data, const uint8_t *specificAddr) {
  esp_now_peer_info_t peerInfo = {};
  memcpy(&peerInfo.peer_addr, specificAddr, 6);
  if (!esp_now_is_peer_exist(specificAddr)) {
    esp_now_add_peer(&peerInfo);
  }

  esp_err_t result = esp_now_send(specificAddr, (const uint8_t *)&data, sizeof(data));

  if (result == ESP_OK) {
    Serial.println("Message sent successfully");
  } else {
    Serial.printf("Error sending message: %s\n", esp_err_to_name(result));
  }
}
