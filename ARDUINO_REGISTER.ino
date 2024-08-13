#include <SPI.h>
#include "RF24.h"

#define CE_PIN 7
#define CSN_PIN 8

RF24 radio(CE_PIN, CSN_PIN);

uint8_t address[] = "00001";

void setup()
{
  Serial.begin(115200);
  while (!Serial)
  {
  }

  if (!radio.begin())
  {
    while (1)
    {
    }
  }

  Serial.println(F("Arduino de Registro"));

  radio.setPALevel(RF24_PA_LOW);

  radio.openWritingPipe(address);

  radio.stopListening();
}

void loop()
{
  Serial.println(F("Digite o login para registro: "));
  while (!Serial.available())
  {
  }
  String login = Serial.readStringUntil('\n');
  login.trim();

  Serial.println(F("Digite a senha para registro: "));
  while (!Serial.available())
  {
  }
  String password = Serial.readStringUntil('\n');
  password.trim();

  String payload = "register:" + login + ":" + password;

  char payloadArray[32];
  payload.toCharArray(payloadArray, 32);

  radio.flush_tx();
  bool report = radio.write(&payloadArray, sizeof(payloadArray));

  if (report)
  {
    Serial.println(F("Solicitação de registro enviada com sucesso!"));
    Serial.println(payload);
  }
  else
  {
    Serial.println(F("Falha ao enviar solicitação de registro."));
  }

  delay(1000);
}
