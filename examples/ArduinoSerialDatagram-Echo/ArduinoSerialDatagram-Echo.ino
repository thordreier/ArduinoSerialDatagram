/*
 * This sketch will send one packet.
 * If ArduinoSerialDatagramHub.py is runnning it should get that packet back,
 * and after one second send it back in reverse.
 * This should continue forever.
 * If ArduinoSerialDatagramHub.py isn't running only one packet will be seen.
 * LED should also blink if packet is bounced back and forth.
 */


#include <ArduinoSerialDatagram.h>

SerialDatagram com;


const int ledPin                = 13;
const uint8_t echoInReversePort = 0xAB;
const uint8_t address           = 0xBB;


void setup()
{
  // Which ports should we listen on and which function should be run if packet is received on that port?
  SerialDatagramListener listeners[] = {
    {echoInReversePort, *echoInReverse}
  };

  // Initialize SerialDatagram
  com.begin(listeners, sizeof(listeners), address);

  while (!Serial) {
    ; // wait for serial port to connect
  }

  // Send first message. The rest of the messages should be responses to this
  // (or responses to responses (or responses to responses to responses (or ...)))
  com.send(address, echoInReversePort, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
}


void loop()
{
  // Run com.update in every loop()
  com.update();
}


// This function will be run if packet is send to port 0xAB (or whatever echoInReversePort is set to).
// The `msg` is a pointer to the decoded byte array. `size` is the number of bytes in `msg`.
// Some of it is borrowed from PacketSerialReverseEcho.ino
void echoInReverse (uint8_t destinationAddress, uint8_t sourceAddress, uint8_t port, const uint8_t* msg, size_t size)
{
  // In this example, we will simply reverse the contents of the array and send
  // it back to the sender.

  // Wait a second. As soon as we send it we should get it back again.
  delay(1000);

  // If first byte is A then turn on LED, else turn off LED
  // Result should be that LED blink every second if packets are flowing
  if (size > 0 && msg[0] == 'A')
  {
    digitalWrite(ledPin, HIGH);
  }
  else
  {
    digitalWrite(ledPin, LOW);
  }

  // Make a temporary buffer.
  uint8_t tempBuffer[size];

  // Copy the packet into our temporary buffer.
  memcpy(tempBuffer, msg, size);

  // Reverse our temporaray buffer.
  reverse(tempBuffer, size);

  // Send the reversed buffer back to the sender.
  // The `tempBuffer` is a pointer to the `tempBuffer` array and `size` is the
  // number of bytes to send in the `tempBuffer`.
  com.send(sourceAddress, port, tempBuffer, size);
}


// This function takes a byte buffer and reverses it.
// Borrowed from PacketSerialReverseEcho.ino
void reverse(uint8_t* buffer, size_t size)
{
  uint8_t tmp;

  for (size_t i = 0; i < size / 2; i++)
  {
    tmp = buffer[i];
    buffer[i] = buffer[size - i - 1];
    buffer[size - i - 1] = tmp;
  }
}
