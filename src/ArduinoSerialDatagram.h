/*
 * https://github.com/thordreier/ArduinoSerialDatagram
 */




// Comment in this line to get more debug output on the serial line
// Make sure this isn't turned on when pushing to git
// Everyting inside "#ifdef ASD_DEBUG" clauses are only compiled if this is set
//#define ASD_DEBUG 1




#ifndef ArduinoSerialDatagram_h
#define ArduinoSerialDatagram_h




// Make it easy to redefine these in the .ino file
#ifndef ASD_BAUD
#define ASD_BAUD           9600
#endif

#ifndef ASD_ADDRESS
#define ASD_ADDRESS        0x00
#endif

#ifndef ASD_EEPROM_ADDRESS
#define ASD_EEPROM_ADDRESS 0xA0
#endif


// These shouldn't be redefined
#define ASD_HEADER_SIZE     4
#define ASD_XSTR(s)         ASD_STR(s)
#define ASD_STR(s)          #s

#include <Arduino.h>
#include <PacketSerial.h>



typedef struct SerialDatagramListener {
    uint8_t  port;
    void    (*function) (uint8_t destinationAddress, uint8_t sourceAddress, uint8_t port, const uint8_t* buffer, size_t size);
} SerialDatagramListener;



class SerialDatagram {
	private:
        // Properties
        static SerialDatagram         *_active;
               PacketSerial           *_packetSerial  = NULL;
               uint8_t                 _address       = 0;
               SerialDatagramListener *_listeners     = NULL;
               uint8_t                 _listenerCount = 0;

        // Methods
        static void    packetHandlerStatic(const uint8_t* packet, size_t size);
               void    packetHandler      (const uint8_t* packet, size_t size);

        #ifdef ASD_DEBUG
               void    debug(const char* str, int number);
        #endif

	public:
        // Methods
               void    begin(SerialDatagramListener *listeners, size_t listenerSize,                             uint8_t address = ASD_ADDRESS, unsigned long speed = ASD_BAUD);
               void    begin(SerialDatagramListener *listeners, size_t listenerSize, PacketSerial *packetSerial, uint8_t address = ASD_ADDRESS);

               void    update();
               uint8_t getAddress();

               void    send(uint8_t address, uint8_t port, const uint8_t* msg, size_t msgSize    );
               void    send(uint8_t address, uint8_t port, const char*    msg, size_t msgSize = 0);
               void    send(uint8_t address, uint8_t port                                        );
               void    log (                               const uint8_t* msg, size_t msgSize    );
               void    log (                               const char*    msg, size_t msgSize = 0);
};




#endif
