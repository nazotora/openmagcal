# Open-Source Magnetometer Calibration
Welcome to the Open-Source Magnetometer Calibration project, made as part of a senior capstone project at the University of Iowa.

The goal of this project was to create a low-cost, open-source design and codebase for a set of triaxial helmholtz coils, to allow college students and non-professionals calibrate magnetometers on their own. As such, this project contains both a software component, found in this repository, and a physical component, the design and assembly details of which can be found here.

## Required items
- Raspberry Pi
  - The model used in the prototype is the Raspberry Pi 400, but any model that has SPI functionality, 3 available GPIO pins, and the ability to connect to a local network through Ethernet or WiFi should work.
- PNI RM3100 magnetometer
- Siglent SPD4121X programmable power supply
  - Any programmable power supply that is SCPI capable through Ethernet or WiFi and has at least 3 output channels should work in theory, with the precision of the current control determining the accuracy of the system.
- 3 3V DPDT relays
  - We used Panasonic TQ2-3V Telecom Relays; any relays that are double-pole-double-throw, can use 3.3V to power the coils, and have a contact rating of at least 1A should work.
- 3D-Printed Helmholtz coil rig, found here: https://www.thingiverse.com/thing:5243483
- 150ft of 22AWG enameled magnet wire
  - 150ft is the **absolute minimum** amount of wire needed to completely wind the Helmholtz coils. However, since it is difficult to wind a coil perfectly and you will need an additional unspecified length of wire to reach a breadboard or circuit board, we recommend getting 200ft of wire to be safe. You will also likely need sandpaper to sand off the enamel on the ends of the wires you're connecting.

Depending on how you want to construct the circuitry for this, you may also need additional breadboards, PCBs, and jumper cables.

## Building
This project relies on the WiringPi library for both the SPI and GPIO control on the Raspberry Pi, which you can download via [the project git repository](https://github.com/WiringPi/WiringPi). By extension, your operating system must also contain the necessary device files to control the SPI and GPIO pins. We highly advise that you use the official Raspberry Pi OS, as that guarantees that the necessary device files and drivers are present.

Then, simply run `make` in the project directory.

### Tests
During the building of this project, multiple tests were developed to verify the functionality of some parts of the code. You can build those via `make test`.

## Assembly
The physical system is made of 5 main sections - the reference magnetometer, the Raspberry Pi, the relays, the Helmholtz coils, and the power supply.

### Reference Magnetometer
The RM3100 specified above is used as the reference magnetometer in the system. The RM3100 should ideally be mounted to some form of breadboard, PCB, or other plate such that it can remain level and pointed in the same direction during operation. The pins on the RM3100 labelled `AVSS`, `DVSS`, and `I2CEN` should be connected to the `GND` pin on the Raspberry Pi. The pins labelled `AVDD` and `DVDD` should be connected to the `3V3` pin on the Raspberry Pi. The `SCK`, `SO`, `SI`, and `SSN` pins form the SPI bus and should be connected to the `SPI0_SCLK`, `SPI0_MISO`, `SPI0_MOSI`, and `SPI0_CE0` pins on the Raspberry Pi. The `DRDY` pin is not used in this system and therefore does not need to be connected to anything.

### Raspberry Pi
The Raspberry Pi will require some form of connection to the same Local Area Network that the power supply is connected to. If the Raspberry Pi used is WiFi-capable, then that may be used; otherwise an ethernet-capable cable should be used to connect it to the local network. If you are not planning to connect to the Raspberry Pi through some form of remote access (e.g. SSH, Telnet), then at the minimum a keyboard and monitor will be required to start the program. If file-mode is being used, then it is possible to disconnect the keyboard and monitor after starting the program as it will automatically terminate on completion of the calibration routine. 

### Relays
Each of the 3 relays should have its T1 and T2 common pins wired to the positive and negative terminal of the corresponding output of the programmable power supply (so that each output is attached to one relay). The 1 and 3 pins on the output should be connected, as well as the 2 and 4 pins. The 1 pin should then be connected to the positive side of the helmholtz coil for a given axis, and the 2 pin to the negative side. This is done with every axis, so that the relay connected to output 1 is also connected to the X-axis coils, output 2 to the Y-axis coils, and output 3 to the Z-axis coils. The positive side of each solenoid should be connected to the GPIO pins of the Raspberry Pi, such that X goes to `GPIO 17`, Y goes to `GPIO 27`, and Z goest to `GPIO 22`. The negative side of the solenoid should be connected to the Raspberry Pi `GND` pin for all 3 relays.

### Helmholtz Coils
The helmholtz coils should be wound in accordance with the right hand rule, with the Z-axis coils being parallel with the ground and the X-axis coils being placed on the smallest diameter rings. The X-axis coils should be wound first, followed by the Y-axis coils, amd finally the Z-axis coils. The coils should be wound in an anticlockwise direction relative to the positive direction of the axis, with 30 loops being made on the negative ring followed by 30 loops on the positive ring. The wire should remain continuous between the 2 loops such that the wire contains a total of 60 anticlockwise loops, split evenly between the 2 rings. Be aware that while it is possible for less than 30 loops to be done per ring, fewer loops will require a higher current per magnetic field created, possibly exceeding safe limits for the wire or relays used.

## Contributors
**Jonah Koch** - Handled power component and hardware research, as well as parts sourcing<br>
**Jared Mulder** - Managed code for magnetometer reading and power supply communication<br>
**Tiger Slowinski** - Handled magnetometer and relay assembly<br>
**Steven Vanni** - Created user interface and file reading code<br>

## Attribution
The Helmholtz coil rig we are using is by [Alexander Schultze](https://www.thingiverse.com/planetsofa/) and is licensed under CC BY 4.0.
