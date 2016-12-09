# ATmega328P (Arduino UNO) Smart Electronic Load #

This ATmega328P-based electronic load is capable of loading power supplies (Batteries, Wallwarts, you name it). It's modes are:

* Constant Current
* Constant Resistance
* Constant Power
* Configurable Pulse Current
* Configurable Triangle Current	(Coming soon!)
* Configurable Sine Current (Coming soon!)



## Getting Started

These instructions will help you build your own Electronic Load. Below you'll find instructions on how to build the circuit as well as the necesary prerequisites on building the arduino code on your machine.

### Code Prerequisites

This Project includes a Rotary Encoder, in order to compile the project you will need to install: 

* [Encoder.h](https://www.pjrc.com/teensy/td_libs_Encoder.html) - Encoder Library from PJRC

### Bill of materials ###

* [MTP3055](http://www.bg-electronics.de/datenblaetter/Transistoren/MTP3055.pdf) - N-Channel **Logic-Level** Mosfet, for replacement parts click [here](http://www.allxref.com/search.htm?part=Mtp3055)
* ATmega328P - And it's required components (16Mhz crystal, 2x 20pF caps, etc)
* LM324N - You could use other compatible op-amps
* Rotary Encoder with center push button
* 16x2 LCD - Hitachi HD44780 compatible LCD
* L7805 - 5v Linear regulator
* 2x mini push button switches
* 10x 10Ω 1% 1W Power resistors
* Various LED's, Resistors and Capacitors


Will be added soon:

* Electronics schematic
* Photos of the build

## TODO

This project is a work in progress, far from complete. Some of the things that will someday be added are:

* Input Protection diodes
* Overvoltage protection at Vsense pins
* Mosfet OverTemperature protection
* Buzzer for audio cues?
* Probably more


## Software used

* [Arduino IDE](https://www.arduino.cc/en/Main/Software) - IDE
* [Proteus 8](https://www.labcenter.com/) - For Schematics and PCB design

## Authors

* **Alejandro Bicelis** - *Coding, electronics* - [abicelis](https://github.com/abicelis)

## License

This project is licensed under the MIT License - see the [LICENSE](https://github.com/abicelis/ElectronicLoad/blob/master/LICENSE) file for details

## Acknowledgments

This project was inspired on David L. Jones's [DIY Constant Current Dummy Load](https://www.youtube.com/watch?v=8xX2SVcItOA) as he published it on his eevblog YouTube Channel.  

