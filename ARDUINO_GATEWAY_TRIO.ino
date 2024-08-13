#include <SPI.h>
#include <EEPROM.h>
#include "RF24.h"

#define CE_PIN 7
#define CSN_PIN 8
#define SERVICE_KEY 123123123

RF24 radio(CE_PIN, CSN_PIN);

uint8_t address[][6] = {"00001", "00002", "00003"};

String maskPass(String password)
{
  String mask = "";
  for (int i = 0; i < password.length(); i++)
  {
    mask += char(password[i] ^ SERVICE_KEY);
  }
  return mask;
}

void writeStringToEEPROM(int addrOffset, const String &strToWrite)
{
  byte len = strToWrite.length();
  EEPROM.write(addrOffset, len);
  for (int i = 0; i < len; i++)
  {
    EEPROM.write(addrOffset + 1 + i, strToWrite[i]);
  }
}

String readStringFromEEPROM(int addrOffset)
{
  int newStrLen = EEPROM.read(addrOffset);
  char data[newStrLen + 1];
  for (int i = 0; i < newStrLen; i++)
  {
    data[i] = EEPROM.read(addrOffset + 1 + i);
  }
  data[newStrLen] = '\0';
  return String(data);
}

int findLoginInEEPROM(String login)
{
  int address = 0;
  while (address < EEPROM.length())
  {
    String storedData = readStringFromEEPROM(address);
    int colonIndex = storedData.indexOf(':');
    String storedLogin = storedData.substring(0, colonIndex);

    if (storedLogin == login)
    {
      return address;
    }

    address += storedData.length() + 1;
  }
  return -1;
}

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

  Serial.println(F("Gateway - Ouvindo solicitações de registro e login"));

  radio.setPALevel(RF24_PA_LOW);

  radio.openReadingPipe(1, address[0]);

  radio.startListening();
}

void loop()
{
  if (radio.available())
  {
    char receivedPayload[32] = {0};
    radio.read(&receivedPayload, sizeof(receivedPayload));

    Serial.print(F("Payload recebido: "));
    Serial.println(receivedPayload);

    String receivedString = String(receivedPayload);
    int firstColon = receivedString.indexOf(':');
    int secondColon = receivedString.indexOf(':', firstColon + 1);

    String type = receivedString.substring(0, firstColon);
    String login = receivedString.substring(firstColon + 1, secondColon);
    String password = receivedString.substring(secondColon + 1);

    if (type == "register")
    {
      int startAddress = findLoginInEEPROM(login);
      if (startAddress == -1)
      {
        String maskPassword = maskPass(password);
        startAddress = EEPROM.length();
        writeStringToEEPROM(startAddress, login + ":" + maskPassword);
        Serial.println(F("Registro feito com sucesso! salvo em EEPROM."));
      }
      else
      {
        Serial.println(F("Falhou. Login já utilizado."));
      }
    }
    else if (type == "login")
    {
      int startAddress = findLoginInEEPROM(login);
      if (startAddress != -1)
      {
        String storedData = readStringFromEEPROM(startAddress);
        int storedColon = storedData.indexOf(':');
        String storedmaskPassword = storedData.substring(storedColon + 1);
        String decryptedStoredPassword = maskPass(storedmaskPassword);

        if (password == decryptedStoredPassword)
        {
          Serial.println(F("Login realizado com sucesso"));
        }
        else
        {
          Serial.println(F("Falha ao fazer Login. Senha incorreta."));
        }
      }
      else
      {
        Serial.println(F("Falha ao fazer Login. Usuário não encontrado."));
      }
    }
  }

  delay(100);
}
