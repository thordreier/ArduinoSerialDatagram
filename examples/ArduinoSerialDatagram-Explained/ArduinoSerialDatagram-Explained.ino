/*
 * Ain't nobody got time for them fancy MarkDown files!
 * This sketch is meant to be documentation with a few examples.
 */


/*
 * The macros are defined like this in ArduinoSerialDatagram.h.
 * If you want to override the values, you can uncomment the lines and change the values.
 * (must be done before #include <ArduinoSerialDatagram.h>)
 */
//#define ASD_BAUD           9600
//#define ASD_ADDRESS        0x00
//#define ASD_EEPROM_ADDRESS 0xA0


/*
 * Include ArduinoSerialDatagram.h and creating a SerialDatagram object should always be done.
 * (unless you do things differently :-) )
 */
#include <ArduinoSerialDatagram.h>
SerialDatagram com;


/*
 * If you choose to use a different serial port (or other custom stuff)
 * you should include PacketSerial.h and create a PacketSerial object.
 * Else remove this stuff.
 */
#include <PacketSerial.h>
PacketSerial packetSerial;


/*
 * Ohh sweet old setup(). Arduino all the way ;-)
 */
void setup()
{
  /*
   * Which ports should we listen on and which function should be run if packet is received on that port?
   * Ports go from 0x00 to 0xFF (256 ports).
   */
  SerialDatagramListener listeners[] = {
    {0x99, *port99},
    {0x88, *port88}
  };

  /*
   * A note about addresses:
   * Valid addresses go from 0x01 to 0xFE (254 addresses) which is way more than the 127 devices that USB support.
   * 0x00 and 0xFF aren't valid addresses that an Arduino can have (there's no-one stopping you!).
   * If you specify 0x00 as the address, the library will try to find the address in EEPROM.
   * It will look at memory address ASD_EEPROM_ADDRESS. That way you can use the same sketch on multiple Arduinos,
   * but addresses come from EEPROM and are therefore different for each device.
   */

  /*
   * OF COURSE, YOU SHOULD ONLY CHOOSE ONE OF THE METHODS BELOW!
   * WEIRD THINGS WILL HAPPEN IF YOU JUST USE THIS CODE AS IS!
   */

  /*
   * This way will take values from ASD_xxx macros. Unless you them it will result in:
   * - Speed will be set to 9600 (ASD_BAUD)
   * - Address will be set to 0x00 (ASD_ADDRESS), and because it's 0x00 it will try to look up the address in EEPROM
   * - Address will come from whatever there may be in EEPROM address 0xA0 (ASD_EEPROM_ADDRESS)
   */
  com.begin(listeners, sizeof(listeners));

  /*
   * This way will set the address to 0x99.
   * (Speed will still be 9600 unless you changed ASD_BAUD)
   * (if you set it to 0x00 it will still try to look up the address in EEPROM)
   */
  com.begin(listeners, sizeof(listeners), 0x99);

  /*
   * This way will set the address to 0x88 and speed to 115200 BAUD.
   * (if you set it to 0x00 it will still try to look up the address in EEPROM)
   */
  com.begin(listeners, sizeof(listeners), 0x88, 115200);

  /*
   * If you can want to use a different serial port then create a PacketSerial object.
   * Look at the documentation for PacketSerial to see how to do that.
   * - Either with default address coming from ASD_ADDRESS
   * - Or with address as parameter
   * (if you set it to 0x00 it will still try to look up the address in EEPROM)
   */
  packetSerial.begin(115200);
  com.begin(listeners, sizeof(listeners), &packetSerial);
  com.begin(listeners, sizeof(listeners), &packetSerial, 0x77);
}


/*
 * Ohh sweet old loop(). Gotta love Arduino for the simplicity!
 */
void loop()
{
  /*
   * Run .update in every loop()
   * If you want to know what happens if it isn't run frequently enough,
   * then look at the documentation for PacketSerial.
   */
  com.update();

  /*
   * Another notes about addresses:
   * If one Arduino is the target of the packet, send it to the address of that device (address between 0x01 and 0xFE).
   * Sending packets to 0x00 will be lost in the void. Or that is, it will be sent to the USB host (hopefully the
   * Raspberry Pi running the ArduinoSerialDatagramHub.py), but it will not be forwarded to any Arduinos.
   * Therefore, this is used to send log messages that can be seen on the Raspberry Pi (if that's what you are using)
   * 0xFF is broadcast. Therefore, use this address if you want to send data to all Arduinos - including itself.
   */

  /*
   * Send packet to port 0x88 on Arduino on address 0x44 with no content.
   * So, in the receiving function, "msg" parameter is empty (and "size" is 0)
   */
  com.send(0x44, 0x88);

  /*
   * Still to port 0x88, though this time it's broadcasted to all Arduinos.
   */
  com.send(0xFF, 0x88);

  /*
   * Still to port 0x88, still broadcasted to all Arduinos, but this time it has a 5 byte long message
   * So, in the receiving function, "msg" is {0x72, 0x65, 0x6C, 0x6C, 0x6F} (and "size" is 5)
   */
  uint8_t msg1[] = {0x72, 0x65, 0x6C, 0x6C, 0x6F};
  com.send(0xFF, 0x88, msg1, sizeof(msg1));

  /*
   * These two does the same. They send a log message to address 0x00, port 0x00 that won't be forwarded to other Arduinos.
   */
  com.log("This is a log message");
  com.send(0x00, 0x00, "This is a log message");

  /*
   * Ain't Nobody Got Time For That!
   */
  delay(10);
}


/*
 * This function will be run if packet is send to port 0x88.
 */
void port88 (uint8_t destinationAddress, uint8_t sourceAddress, uint8_t port, const uint8_t* msg, size_t size)
{
  if (! size)
  {
    com.log("Empty! I deserve more than that!");
  }
  else if (size == 5)
  {
    if (msg[0] == 0x72)
    {
      com.log("First byte was what I hoped for. And five long. Keep em coming!");
    }
    else
    {
      com.log("I did not see that coming.");
    }
  }
  else
  {
    com.log("Got something that was not 5 bytes long - but not empty. Why are you doing this to me?");
  }
}


/*
 * This function will be run if packet is send to port 0x99.
 */
void port99 (uint8_t destinationAddress, uint8_t sourceAddress, uint8_t port, const uint8_t* msg, size_t size)
{
  // Do stuff in here
}
