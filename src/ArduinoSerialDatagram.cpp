/*
 * https://github.com/thordreier/ArduinoSerialDatagram
 */



#include <Arduino.h>
#include <EEPROM.h>
#include <PacketSerial.h>
#include "ArduinoSerialDatagram.h"




// Initialize static class property
SerialDatagram *SerialDatagram::_active = NULL;




#ifdef ASD_DEBUG
void SerialDatagram::debug(const char* str, int number = 0)
{
    char msg[128];
    sprintf(msg, str, number);
    log(msg);
}
#endif




void SerialDatagram::begin(SerialDatagramListener *listeners, size_t listenerSize, uint8_t address, unsigned long speed)
{
    PacketSerial *packetSerial = new PacketSerial;
    packetSerial->begin(speed);
    begin(listeners, listenerSize, packetSerial, address);
}




void SerialDatagram::begin(SerialDatagramListener *listeners, size_t listenerSize, PacketSerial *packetSerial, uint8_t address)
{
    _packetSerial = packetSerial;
    _packetSerial->setPacketHandler(&SerialDatagram::packetHandlerStatic);

    if (address == 0x00)
    {
        _address = EEPROM.read(ASD_EEPROM_ADDRESS);
    }
    else
    {
        _address = address;
    }

    _listenerCount = listenerSize / sizeof(SerialDatagramListener);
    _listeners = (SerialDatagramListener*) malloc(listenerSize);
    memcpy(_listeners, listeners, listenerSize);
}




void SerialDatagram::update()
{
    _active = this;
    _packetSerial->update();
}




void SerialDatagram::packetHandlerStatic(const uint8_t* packet, size_t size)
{
    _active->packetHandler(packet, size);
}




void SerialDatagram::packetHandler(const uint8_t* packet, size_t size)
{
    #ifdef ASD_DEBUG
    debug("Packet size received: <%0d>.", size);
    #endif
    
    if (size >= ASD_HEADER_SIZE)
    {
        uint8_t crc = 0x00;
        for (size_t i = 0; i < size; i++)
        {
            crc ^= packet[i];
        }

        if (crc == 0x00)
        {
            #ifdef ASD_DEBUG
            debug("Destination address: <0x%02X>.", packet[0]);
            debug("Source address:      <0x%02X>.", packet[1]);
            debug("Destination port:    <0x%02X>.", packet[2]);
            #endif

            if (packet[0] == _address || packet[0] == 0xFF)
            {
                // Packet is send to my address or broadcast address

                #ifdef ASD_DEBUG
                debug("Packet is for me.");
                #endif

                for (uint8_t i = 0; i < _listenerCount; i++)
                {
                    if (_listeners[i].port == packet[2])
                    {
                        #ifdef ASD_DEBUG
                        debug("Found listener for port <0x%02X>.", packet[2]);
                        #endif

                        _listeners[i].function(packet[0], packet[1], packet[2], packet + ASD_HEADER_SIZE, size - ASD_HEADER_SIZE);
                    }
                }
            }
            #ifdef ASD_DEBUG
            else
            {
                debug("Packet is for someone else.");
            }
            #endif
        }
        #ifdef ASD_DEBUG
        else
        {
            debug("CRC error in packet.");
        }
        #endif
    }
    #ifdef ASD_DEBUG
    else
    {
        debug("Packet too small, below " ASD_XSTR(ASD_HEADER_SIZE) " bytes.");
    }
    #endif
}




uint8_t SerialDatagram::getAddress()
{
    return _address;
}




void SerialDatagram::send(uint8_t address, uint8_t port, const uint8_t* msg, size_t msgSize)
{
    size_t packetSize = msgSize + ASD_HEADER_SIZE;
    uint8_t packet[packetSize] = {address, _address, port, 0x00};

    memcpy(packet + ASD_HEADER_SIZE, msg, msgSize);
    
    uint8_t crc = 0x00;
    for (size_t i = 0; i < packetSize; i++)
    {
        crc ^= packet[i];
    }
    packet[3] = crc;

    _packetSerial->send(packet, packetSize);
}




void SerialDatagram::send(uint8_t address, uint8_t port, const char* msg, size_t msgSize)
{
    if (msgSize == 0) {
        msgSize = strlen(msg);
    }

    send(address, port, (unsigned char *) msg, msgSize);
}




void SerialDatagram::send(uint8_t address, uint8_t port)
{
    send(address, port, (unsigned char *) {}, 0);
}




void SerialDatagram::log(const uint8_t* msg, size_t msgSize)
{
    send(0x00, 0x00, msg, msgSize);
}




void SerialDatagram::log(const char* msg, size_t msgSize)
{
    send(0x00, 0x00, msg, msgSize);
}
