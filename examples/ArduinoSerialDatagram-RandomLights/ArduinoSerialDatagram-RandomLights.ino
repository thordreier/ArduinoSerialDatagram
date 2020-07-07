#include <ArduinoSerialDatagram.h>

SerialDatagram com;

const int ledPin       = 13;
const int helloPort    = 0x20;
const int lightOnPort  = 0xAB;
const int lightOffPort = 0xAC;
const int firstAddress = 0x41;
const int lastAddress  = 0x49;

int helloCountdown    = 1;
int lightOnCountdown  = 1;
int lightOffCountdown = 1;


void setup()
{
  randomSeed(analogRead(0));

  // Which ports should we listen on and which function should be run if packet is received on that port
  // Ports go from 0x00 to 0xFF (256 ports)
  SerialDatagramListener listeners[] = {
    {helloPort,    *hello},
    {lightOnPort,  *lightOn},
    {lightOffPort, *lightOff}
  };

  // Which address should this Arduino have
  // Valid adresses goes from 0x01 to 0xFE (254 addresses) which is way more than the 127 devices that USB support
  // Here we generate a random address between 0x41 and 0x49 (both included)
  uint8_t address = random(firstAddress, lastAddress + 1);
  
  com.begin(listeners, sizeof(listeners), address);

  // initialize the LED pin as an output
  pinMode(ledPin, OUTPUT);
}


void loop()
{
  // Run this once in every loop
  com.update();
  
  if (! --helloCountdown)
  {
    uint8_t destinationAddress = random(firstAddress, lastAddress + 1);
    char msg[32];
    sprintf(msg, "Hello 0x%02X, are you there?", destinationAddress);
    com.send(destinationAddress, helloPort, msg);
    helloCountdown = random(20, 100);
  }

  if (! --lightOnCountdown)
  {
    com.send(0xFF, lightOnPort);
    lightOnCountdown = random(20, 100);
  }

  if (! --lightOffCountdown)
  {
    com.send(0xFF, lightOffPort);
    lightOffCountdown = random(20, 100);
  }

  delay(100);
}


void hello (uint8_t destinationAddress, uint8_t sourceAddress, uint8_t port, const uint8_t* msg, size_t size)
{
  com.log("Uuuuuuuuuuuhhhhhhhh you found me. I thought I was invisible! Damn!");
}


void lightOn (uint8_t destinationAddress, uint8_t sourceAddress, uint8_t port, const uint8_t* msg, size_t size)
{
  digitalWrite(ledPin, HIGH);
}


void lightOff (uint8_t destinationAddress, uint8_t sourceAddress, uint8_t port, const uint8_t* msg, size_t size)
{
  digitalWrite(ledPin, LOW);
}
