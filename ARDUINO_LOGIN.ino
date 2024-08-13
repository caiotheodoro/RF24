#include <SPI.h>
#include "RF24.h"

#define CE_PIN 7
#define CSN_PIN 8

RF24 radio(CE_PIN, CSN_PIN);

uint8_t address[] = "00001";  

void setup() {
  Serial.begin(115200);
  while (!Serial) {}

  if (!radio.begin()) {
    Serial.println(F("radio hardware is not responding!!"));
    while (1) {}
  }

  Serial.println(F("RF24 Login Arduino"));

  radio.setPALevel(RF24_PA_LOW);

  radio.openWritingPipe(address);
  radio.openReadingPipe(1, address);

  radio.stopListening();
}

void loop() {
  Serial.println(F("Enter login for authentication: "));
  while (!Serial.available()) {}
  String login = Serial.readStringUntil('\n');
  login.trim();

  Serial.println(F("Enter password for authentication: "));
  while (!Serial.available()) {}
  String password = Serial.readStringUntil('\n');
  password.trim();

  String payload = "login:" + login + ":" + password;

  char payloadArray[32];
  payload.toCharArray(payloadArray, 32);

  radio.flush_tx();
  bool report = radio.write(&payloadArray, sizeof(payloadArray));

  if (report) {
    Serial.println(F("Login request sent successfully!"));
    Serial.println(payload);

    radio.startListening();

    unsigned long startedWaitingAt = millis();
    bool timeout = false;
    while (!radio.available()) {
      if (millis() - startedWaitingAt > 3000) {  
        timeout = true;
        break;
      }
    }

    if (timeout) {
      Serial.println(F("Response timeout."));
    } else {
      char response[32] = {0};
      radio.read(&response, sizeof(response));

      if (String(response) == "login_success") {
        Serial.println(F("Login successful!"));
      } else if (String(response) == "login_failed") {
        Serial.println(F("Login failed. Incorrect login or password."));
      }
    }

    radio.flush_rx();
    radio.stopListening();
  } else {
    Serial.println(F("Login request failed or timed out"));
  }

  delay(1000);
}
