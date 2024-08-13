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

  Serial.println(F("RF24 Register Arduino"));

  radio.setPALevel(RF24_PA_LOW);

  radio.openWritingPipe(address);

  radio.stopListening();
}

void loop() {
  Serial.println(F("Enter login for registration: "));
  while (!Serial.available()) {}
  String login = Serial.readStringUntil('\n');
  login.trim();

  Serial.println(F("Enter password for registration: "));
  while (!Serial.available()) {}
  String password = Serial.readStringUntil('\n');
  password.trim();

  String payload = "register:" + login + ":" + password;

  char payloadArray[32];
  payload.toCharArray(payloadArray, 32);

  radio.flush_tx();
  bool report = radio.write(&payloadArray, sizeof(payloadArray));

  if (report) {
    Serial.println(F("Registration request sent successfully!"));
    Serial.println(payload);
  } else {
    Serial.println(F("Registration request failed or timed out"));
  }

  delay(1000);
}
