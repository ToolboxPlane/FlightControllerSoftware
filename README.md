# FlightController
## Building and deploy
### Compile
To compile the firmware run:
```
cmake . && make
```
To compile the tests run the same commands in the test directory 
#### WTF? Two independent cmake-projects?
You may ask why is there a second, independet cmake project for the tests (and you have the best right to do so). 
Well i do not like the solution either but it seems like cmake is not able to have two targets with different compilers. 
So if you find a better solution for doing this (no i won't use Makefiles or Bazel or Scons or Gradle or whatever...) please write an issue and i will fix this immediatly.

### How to flash to firmware
This assumes that an AVRISP mk2 is used, if you use a different programmer
change the ```-c``` flag.
```bash
avrdude -p m2560 -B 10 -c avrisp2 -U flash:w:FlightController.hex:i
```

### Fuses
The fuses select the "Ext. Crystal Osc.; Frequency 8.0-    MHz; Start-up time: 16K CK + 4.1 ms; [CKSEL=1111 SUT=10]"
clock, disables the clock divider, enables the preserve eeprom flag, enables the watchdog timer, enables SPI and disables
JTAG and OCD. The brownout detection level is set to 1.8V.

| Fusebyte | Value |
|--- | --- |
| Low | 0xEF |
| High | 0xC1 |
| Extended | 0xFE |

#### How to flash the fuses
```bash
avrdude -p m2560 -B 10 -c avrisp2 -U lfuse:w:0xef:m -U hfuse:w:0xc1:m -U efuse:w:0xfe:m
```

## LEDs
| LED-Number | Meaning |
| --- | --- |
| 0 | Alive (blinks regularly) |
| 1 | Not Watchdog |
| 2 | Not Brownout |
| 3 | USB-RX (Toggle on package receive) |
| 4 | USB-TX (Toggle on package send) |
| 5 | SBUS-RX (Toggle on package receive) |
| 6 | Use Flightcomputer as a setpoint source |
| 7 | Not Failsave |

## Package format
### FlightController Output (Transmitter ID 23)
The output package is a 10 bit, 16 Channel Package with the following data:

| Channel | Data |
| --- | --- |
| 0 | Heading (value between 0 and 360) |
| 1 | Roll + 180 |
| 2 | Pitch + 180 |
| 3 | Empty |
| 4 | Empty |
| 5 | Empty |
| 6 | Acceleration-Forward + 500 |
| 7 | Acceleration-Sideward +500 |
| 8 | Acceleration-UpDown + 500 |
| 9 | Empty |
| 10 | Empty |
| 11 | Servo-Aileron-Right +500 |
| 12 | Servo-VTail-Right +500 |
| 13 | Motor + 500 |
| 14 | Servo-VTail-Left + 500 |
| 15 | Servo-Aileron-Left + 500|

### SBus Output (Transmitter ID 56)
The package is a 11 bit, 16 Channel Package, with the same information as the sbus packagethe same information as the sbus package.

## What are all these different structs?
 * `state`: The current state of the airplane as measured by the BNO055
 * `out_state`: The (desired) state of the outputs (servos and motors)
 * `setpoint`: The setpoint sent by the flightcomputer (power, pitch, roll)
 
## Coordinate system
All coordinates form a right-handed trihedron. The x axis points forward, the y axis to the left side 
and the z axis upward.