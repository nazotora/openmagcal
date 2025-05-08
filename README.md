# Open-Source Magnetometer Calibration
Welcome to the Open-Source Magnetometer Calibration project, made as part of a senior capstone project at the University of Iowa.
The goal of this project was to create a low-cost, open-source design and codebase for a set of triaxial helmholtz coils, to allow college students and non-professionals calibrate magnetometers on their own. As such, this project contains both a software component, found in this repository, and a physical component, the design and assembly details of which can be found here.
## Required items
- Raspberry Pi
  - The model used in the prototype is the Raspberry Pi 400, but any model that has SPI functionality, 3 available GPIO pins, and the ability to connect to a local network through Ethernet or WiFi should work.
- PNI RM3100 Magnetometer
- Siglent SPD4121X Programmable Power Supply
  - Any Programmable Power Supply that is SCPI capable through Ethernet or WiFi and has at least 3 output channels should work in theory, with the precision of the current control determining the accuracy of the system.
- 3 3V DPDT relays
  - We used Panasonic TQ2-3V Telecom Relays, any relays that are double-pole-double-throw, can use 3.3V to power the coils, and have a contact rating of at least 1A should work.
- 3D-Printed Helmholtz Coil Rig, found here:
- 150ft of 22AWG Enameled Magnet Wire
  - 150ft is the **absolute minimum** amount of wire needed to completely wind the Helmholtz coils. However, since it is difficult to wind a coil perfectly and you will need an additional unspecified length of wire to reach a breadboard or circuit board, we recommend getting 200ft of wire to be safe.
Dependent on how you would want to construct the circuitry for this, you may need additional breadboards, PCBs, and jumper cables.
## Assembly
The physical system is made of 5 main sections - The reference magnetometer, the Raspberry Pi, the relays, the Helmholtz coils, and the power supply.
### Reference Magnetometer
The RM3100 specified above is used as the reference magnetometer in the system. The RM3100 should ideally be mounted to some form of breadboard, PCB, or other plate such that it can remain level and pointed in the same direction during operation. The pins on the RM3100 labelled `AVSS`, `DVSS`, and `I2CEN` should be connected to the `GND` pin on the Raspberry Pi. The pins labelled `AVDD` and `DVDD` should be connected to the `3V3` pin on the Raspberry Pi. The `SCK`, `SO`, `SI`, and `SSN` pins form the SPI bus and should be connected to the `SPI0_SCLK`, `SPI0_MISO`, `SPI0_MOSI`, and `SPI0_CE0` pins on the Raspberry Pi. The `DRDY` pin is not used in this system and therefore does not need to be connected to anything.
