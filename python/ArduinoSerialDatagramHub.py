#!/usr/bin/python3


# Install cobs:
# pip3 install cobs


import argparse
import serial
import signal
import sys
import threading
import time
from cobs import cobs


class Port:
    ports = []

    def __init__(self, path, speed):
        self.serial = serial.Serial(port = path, baudrate = speed)
        ######self.serial.reset_input_buffer()
        self.lock = threading.Lock()
        self.address = 0x00

    def read(self):
        while True:
            data = self.serial.read_until(b'\x00')
            self.process(data)

    def write(self, data):
        with self.lock:
            self.serial.write(data)

    def process(self, data):
        try:
            real = cobs.decode(data[:-1])
            if len(real) > 3:
                if self.crc(real):
                    if self.address != real[1]:
                        self.address = real[1]
                        print('               ADDRESS 0x{:02X} IS ON PORT {}'.format(self.address, self.serial.port))
                    print('0x{:02X} 0x{:02X} 0x{:02X} {}'.format(real[1], real[0], real[2], real[4:]))
                    if real[0] != 0x00:
                        to = []
                        for port in self.ports:
                            if real[0] == 0xFF or real[0] == port.address or port.address == 0x00:
                                to.append('{}(0x{:02X})'.format(port.serial.port, port.address))
                                threading.Thread(target = port.write, args=(data,), daemon = True).start()
                        if not to:
                            to.append("NO-ONE")
                        print('               -> ' + ' '.join(to))
                else:
                    print('               CRC DROP: {}'.format(real))
            else:
                print('               LEN DROP: {}'.format(real))
        except:
            print('               COBS DROP: {}'.format(data))

    @classmethod
    def start(cls, paths, speed = 9600):
        print("SRC  DST  PORT MSG")
        print('               SPEED: {} BAUD'.format(speed))
        for path in paths:
            print('               LISTENING ON PORT: {}'.format(path))
            cls.ports.append(Port(path, speed))
        time.sleep(2)
        for port in cls.ports:
            threading.Thread(target = port.read, daemon = True).start()

    @staticmethod
    def crc(data):
        crc = 0x00
        for b in data:
            crc ^= b
        return crc == 0x00


argParse = argparse.ArgumentParser(description = 'Forward packets between Arduinos',
                                   formatter_class = argparse.ArgumentDefaultsHelpFormatter)
argParse.add_argument('-s', '--speed', type = int, default = 9600, help = 'Serial speed in BAUD')
argParse.add_argument('devs', metavar = 'DEV', nargs = '*', help = 'Seriel device, eg. /dev/ttyACM1')
args = argParse.parse_args()

Port.start(args.devs, args.speed)

try:
    signal.pause()
except KeyboardInterrupt:
    sys.exit()
