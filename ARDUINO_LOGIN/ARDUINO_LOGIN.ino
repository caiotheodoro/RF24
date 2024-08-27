#include <SPI.h>
#include "RF24.h"

#define CE_PIN 7
#define CSN_PIN 8

#define MSG 0
#define ACK 1
#define RTS 2
#define CTS 3


RF24 radio(CE_PIN, CSN_PIN);

uint8_t address[] = "00001X1X1X1X1";

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


  radio.setChannel(72);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_2MBPS);

  radio.openWritingPipe(address);
  radio.openReadingPipe(1, address);  // Open a reading pipe to receive ACK

  radio.setAutoAck(false);

  radio.stopListening();
}

bool sendPacket(byte *pacote, int tamanho, int destino, int controle)
{
  while (1)
  {
    radio.startListening();
    delayMicroseconds(70);
    radio.stopListening();
    if (!radio.testCarrier())
    {
      radio.write(pacote, tamanho);
      return true;
    }
    else
    {
      delayMicroseconds(270);
    }
    radio.flush_rx();
  }
}

bool aguardaMsg(int tipo)
{
  radio.startListening();
  unsigned long tempoInicio = millis();
  while (millis() - tempoInicio < 500)
  {
    if (radio.available())
    {
      char receivedPayload[32] = {0};
      radio.read(&receivedPayload, sizeof(receivedPayload));
      String receivedString = String(receivedPayload);
      int firstColon = receivedString.indexOf(':');
      String type = receivedString.substring(0, firstColon);
      if (type.toInt() == tipo)
      {
        return true;
      }
    }
  }
  return false;
}

void loop()
{
  Serial.println(F("Digite o login: "));
  while (!Serial.available())
  {
  }
  String login = Serial.readStringUntil('\n');
  login.trim();

  Serial.println(F("Digite a senha: "));
  while (!Serial.available())
  {
  }
  String password = Serial.readStringUntil('\n');
  password.trim();

  String payload = "login:" + login + ":" + password;

  char payloadArray[32];
  payload.toCharArray(payloadArray, 32);

  radio.flush_tx();
  
  bool report = sendPacket((byte *)payloadArray, sizeof(payloadArray), payloadArray[0], MSG);

  if (report)
  {
    Serial.println(payload);

    radio.startListening();
    report = aguardaMsg(ACK);
    if (report)
    {
      report = sendPacket((byte *)payloadArray, sizeof(payloadArray), payloadArray[0], ACK);
      if (report)
      {
        Serial.println(F("ACK enviado ao gateway"));
      }
      else
      {
        Serial.println(F("Falha ao enviar ACK ao gateway"));
      }
    }
    else
    {
      Serial.println(F("Nenhum CTS recebido: Falha na comunicação com o gateway."));
    }
  }
  else
  {
    Serial.println(F("Falha ao enviar solicitação de login."));
  }

  delay(1000);
}
