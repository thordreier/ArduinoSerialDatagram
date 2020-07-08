# ArduinoSerialDatagram

Send data between multiple Arduino on USB bus, with a Raspberry Pi as host.


## Why

There's a lot of ways to send data between multiple Arduinos.

But if you already got them all hooked up on an USB bus (maybe for power, maybe to ease uploading new sketches) wouldn't it be nice to send data between them over that USB bus?

The problem is that you can't do that: The Arduino is an USB slave, and for that to work you need an USB host on the bus (well, maybe you could with an Arduino USB Host Shield or something like that - and that would maybe introduce new problems if you used Arduino clones: getting drivers for the weird serial chips).

But if you got it all hooked up to a small computer you could just use that as the USB host.

I have personally used an old Raspberry Pi B+ (the model from 2014) that I didn't use for anything else because it was too slow.

The benefit of this setup is that only one cable delivers both power and communication and that cheap USB hubs can be used if more devices should be attached.

At the same time uploading a new sketch to the Arduino is easy, it can just be done from the Raspberry Pi - the Arduinos are already connected.


## Install

Start by installaling the library (and PacketSerial too). Here we instal both using git:

```
# cd to the Arduino Libraries directory
cd ~/Arduino/libraries/

# Get PacketSerial
git clone https://github.com/bakercp/PacketSerial.git

# Get ArduinoSerialDatagram
git clone https://github.com/thordreier/ArduinoSerialDatagram.git
```

There's a Python script that will forward packets between the Arduinos. If you put something like this in `/etc/rc.local` it will start at boot time and you can use `screen -x ArduinoSerialDatagram` to see the status (if you don't know `screen` here's a hint: use Ctrl-a + Ctrl-d to detach without stopping what's running).

```
su - pi -c 'cd ~/Arduino/libraries/ArduinoSerialDatagram/python/ && screen -dm -S ArduinoSerialDatagram ./ArduinoSerialDatagramHub.sh'
```


## Coding

I'm too lazy to write code examples here in MarkDown. Take a look at the [example files](examples/). Especially [ArduinoSerialDatagram-Explained.ino](examples/ArduinoSerialDatagram-Explained/ArduinoSerialDatagram-Explained.ino).

