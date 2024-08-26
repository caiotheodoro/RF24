#include <SPI.h>
#include <EEPROM.h>
#include "RF24.h"

//desabilitar o auto ack,   
// /login, registro e gateway mesmo endereco
//base para protocolo 
//https://github.com/phsabo/OPTATIVA/blob/main/coordenado_aula.ino

#define CE_PIN 7
#define CSN_PIN 8
#define SERVICE_KEY 123123123

#define MSG 0
#define ACK 1
#define RTS 2
#define CTS 3

RF24 radio(CE_PIN, CSN_PIN);

uint8_t address[] = "00001X1X1X1X1";

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
  radio.setChannel(15);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_2MBPS);
  radio.openReadingPipe(1, address);//colocar todos ndoes no mesmo pipe
  radio.openWritingPipe(address);

  radio.startListening();

  radio.setAutoAck(false);
}



bool aguardaMsg(int tipo){
    radio.startListening();
    unsigned long tempoInicio = millis();
    while(millis()-tempoInicio<500){
      if (radio.available()) {    
        char receivedPayload[32] = {0};
        radio.read(&receivedPayload, sizeof(receivedPayload));
        String receivedString = String(receivedPayload);
        int firstColon = receivedString.indexOf(':');
        int secondColon = receivedString.indexOf(':', firstColon + 1);
        String type = receivedString.substring(0, firstColon);
        if(type==tipo){
          return true;
        }
      }
    }
    return false;
}

bool sendPacket(byte *pacote, int tamanho){
  while(1){
      radio.startListening();
       delayMicroseconds(70);
       radio.stopListening();
       if (!radio.testCarrier()) {
          Serial.println("Write Success");
          return radio.write(&pacote[0], tamanho);
       }
       else {
          Serial.println("Meio ocupado");
          delayMicroseconds(270);
       }
        radio.flush_rx();
  }
}
void loop()
{
  radio.startListening();
  if (radio.available())
  {
    char receivedPayload[32] = {0};
    
    Serial.print(F("Payload recebido"));
    radio.read(&receivedPayload, sizeof(receivedPayload));
    Serial.print(F("Payload recebido: "));
    Serial.println(receivedPayload);

    String receivedString = String(receivedPayload);
    int firstColon = receivedString.indexOf(':');
    int secondColon = receivedString.indexOf(':', firstColon + 1);
    int thirdColon = receivedString.indexOf(':', secondColon + 1);

    String type = receivedString.substring(0, firstColon);
    String login = receivedString.substring(firstColon + 1, secondColon);
    String password = receivedString.substring(secondColon + 1);
    String ctsOrRts = receivedString.substring(thirdColon + 1);

    if (type == "register")
    {
      int startAddress = findLoginInEEPROM(login);
      if (startAddress == -1)
      {
        String maskPassword = maskPass(password);
        startAddress = EEPROM.length();
        writeStringToEEPROM(startAddress, login + ":" + maskPassword);
        Serial.println(F("Registro feito com sucesso! salvo em EEPROM."));

        bool report = sendPacket((byte *)(ACK + ":1"), 6);
      }
      else
      {
        Serial.println(F("Falhou. Login já utilizado."));

        bool report = sendPacket((byte *)(ACK + ":0"), 6);

        
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

          bool report = sendPacket((byte *)(ACK+":1"), 6);

        }
        else
        {
          Serial.println(F("Falha ao fazer Login. Senha incorreta."));

          bool report = sendPacket((byte *)(ACK+":0"), 6);
        }
      }
      else
      {
        Serial.println(F("Falha ao fazer Login. Usuário não encontrado."));

        bool report = sendPacket((byte *)(ACK+":2"), 6);
      }
    }
  }

  delay(100);
}