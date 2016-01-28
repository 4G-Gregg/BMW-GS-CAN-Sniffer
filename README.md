# BMW R1200GS CAN-Bus Sniffer

## Objective
Create a device capable of sniffing the CAN Bus of a 2010 BMW R1200 GS motorcycle. This will be accomplished using an Arduino and CAN Bus shield and appending the sniffer to the motorcycle's CAN Bus.

## Current Demo
![CAN Bus YouTube Demo](https://j.gifs.com/2k85pM.gif)<br>
[Full Video on YouTube](https://youtu.be/-c5wJQV1saM)

## Applications
### [Accessory_Controller](Accessory_Controller/)
This will be the main application for driving the LED auxiliary
light and heated jacket implementation.

### [Event_Printer](Event_Printer/)
Prints out all known events to either the TFT display or the
serial terminal. This is mostly used for debugging, demos, and
new message discovery.

### [Sniff_Arduino_Terminal](Sniff_Arduino_Terminal/)
Prints all the messages raw to the terminal in a rough CSV form. This
is useful for finding new messages.

## Hardware
### [Arduino Uno](https://www.arduino.cc/en/Main/ArduinoBoardUno)<br>
![Arduino](http://www.arduino.cc/en/uploads/Main/ArduinoUno_R3_Front_450px.jpg)
### [Seeeduino CAN Bus Shield](http://www.seeedstudio.com/wiki/CAN-BUS_Shield_V1.2)<br>
![CAN Bus Shield](http://www.seeedstudio.com/wiki/images/thumb/b/b0/CAN-BUS_Shield_V1.2.jpg/400px-CAN-BUS_Shield_V1.2.jpg)

## Software
- [Seeeduino CAN Bus Shield Library](https://github.com/Seeed-Studio/CAN_BUS_Shield)
- [SparkFun CAN-Bus Arduino Library](https://github.com/sparkfun/SparkFun_CAN-Bus_Arduino_Library)

## Connecting to the Alarm wiring harness
The extremely overpriced BMW repair plug and wiring harness can be [purchased here](https://www.maxbmwmotorcycles.com/fiche/PartsSearch.aspx?&searchtype=partnumber&parts=83300413581).

### Pinout on the Alarm harness
- 3: Switched 12v Rail
- 4: Ground
- 5: CAN Low
- 6: CAN High

## Protocol Parsing
There is a group of people working on reverse engineering the CAN bus messages
in a [Google Spreadsheet](https://docs.google.com/spreadsheets/d/1tUrOES5fQZa92Robr6uP8v2dzQDq9ohHjUiTU3isqdc/edit#gid=0).
I've used this as the basis for my parsing, but have
found some of the information to be incorrect.

## TFT Display
One of the more "whiz-bang" features I added was a
[TFT display](https://learn.adafruit.com/1-8-tft-display/breakout-assembly)
that allows me to see the status of the bike without staring at
the computer screen. It most likely won't be used in the final
implementation but I thought it was a cool demo.

## Prototype Case for Arduino with the CAN Bus Shield
[The Congers CANBusGateway Project](http://development.thecongers.org/home/canbusgateway) had a case design that I modified slightly to use the CAN Bus screw down connections instead of the DB9 connector. Those files are included [in this repo](Mechanicals/).

![3D Printed Prototype Case](http://i.imgur.com/rbw4ONJ.jpg)

## Interfacing With Accessories
###[SparkFun Monster Motor Shield](https://www.sparkfun.com/products/10182)<br>
![SparkFun Monster Motor Shield](https://cdn.sparkfun.com//assets/parts/4/4/9/1/10182-01.jpg)

## Resources
- [The Congers CANBusGateway Project](http://development.thecongers.org/home/canbusgateway)
- [ADVRider CAN Bus Reversing Thread](http://advrider.com/index.php?threads/1200gs-canbus-reversing.1063196/)
- [K1600 Forum - CAN Bus Adventure](http://www.k1600forum.com/forum/bmw-k1600-maintenance-do-yourself/3440-canbus-adventure-begins-2.html)
- [R1200GS Wiring Diagram](http://www.r1200gs.info/R1200GS-WD2.pdf)
- [Seeeduino CAN Bus Shield Wiki](http://www.seeedstudio.com/wiki/CAN-BUS_Shield_V1.2)
- [Bluetooth Serial Link](https://www.adafruit.com/products/1588)
- [S1000RR OBDII Reader Project](http://www.s1000rrforum.com/forum/s1000rr-hp4-do-yourself/19954-diy-obd2-s100rr-extended-display-gauges-obd2-code-reader.html)
