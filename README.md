# ATmega328P (Arduino UNO) Electronic Load #

This ATmega328P-based electronic load is capable of loading power supplies (Batteries, Wallwarts, you name it). It's modes are:

* Constant Current
* Constant Resistance
* Constant Power
* Configurable Pulse Current
* Configurable Triangle Current	(Coming soon!)
* Configurable Sine Current (Coming soon!)


## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. See deployment for notes on how to deploy the project on a live system.


### Bill of materials ###

* [MTP3055](http://www.bg-electronics.de/datenblaetter/Transistoren/MTP3055.pdf) - N-Channel **Logic-Level** Mosfet, for replacement parts click [here](http://www.allxref.com/search.htm?part=Mtp3055)
* ATmega328P, 16Mhz crystal, 2x 20pF caps.
* LM324N - You could use other compatible op-amps
* Rotary Encoder with center push button
* 16x2 LCD - Hitachi HD44780 compatible LCD
* L7805 - 5v Linear regulator
* 2x mini push button switches
* 10x 10Ω 1% 1W Power resistors
* Various LED's, Resistors and Capacitors



Will be added soon:

* Electronics schematic.
* Photos of the build.





### Prerequisites

This Project includes a Rotary Encoder, in order to compile the project you will need to install: 

* [Encoder.h](https://www.pjrc.com/teensy/td_libs_Encoder.html) - Encoder Library from PJRC

## Built With

* [Arduino IDE](https://www.arduino.cc/en/Main/Software) - IDE
* [Proteus 8](https://www.labcenter.com/) - For Schematics and PCB design

## Authors

* **Alejandro Bicelis** - *Coding* - [abicelis](https://github.com/abicelis)

## License

This project is licensed under the MIT License - see the [LICENSE](https://github.com/abicelis/ElectronicLoad/blob/master/LICENSE) file for details

## Acknowledgments

This project was inspired on David L. Jones's [DIY Constant Current Dummy Load](https://www.youtube.com/watch?v=8xX2SVcItOA) as he published it on his eevblog YouTube Channel.  

