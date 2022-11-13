# FlightController
## Building and deploy
### Compile
To compile the firmware run:
```
cmake . && make
```
To compile the tests run the same commands in the test directory 

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
Under normal use all leds should be either on or blinking.
By inverting most imu_datas one can be sure that all leds are working.

If only a single led is off for a longer time, there is a some kind of problem
(with the small exception of the sbus-override).

| LED-Number | Meaning |
| --- | --- |
| 0 | Alive (blinks regularly) |
| 1 | Not Watchdog |
| 2 | Not Brownout |
| 3 | Timer active  |
| 4 | USB-RX (Toggle on package receive) |
| 5 | SBUS-RX (Toggle on package receive) |
| 6 | Use Flightcomputer as a setpoint source |
| 7 | Not Failsave |

## Package format
The UART-Baud Rate is 115200.
### FlightController Output (Transmitter ID 23)
The output package is a 10 bit, 16 Channel Package with the following data:

The axis are according to DIN-9300.

| Channel | Data |
| --- | --- |
| 0 | BNO055 State |
| 1 | Roll * 2 + 500|
| 2 | Pitch * 2+ 500 |
| 3 | Yaw * 2 + 500 |
| 4 | d/dt Roll + 500 |
| 5 | d/dt Pitch + 500 |
| 6 | d/dt Heading + 500 |
| 7 | Acceleration-X * 6.25 + 500 |
| 8 | Acceleration-Y * 6.25 + 500 |
| 9 | Acceleration-Z * 6.25 + 500 |
| 10 | BNO055 Error |
| 11 | BNO055 Calib-Status |
| 12 | Empty |
| 13 | Motor |
| 14 | Servo-Elevon-Left + 500 |
| 15 | Servo-Elevon-Right + 500|

### SBus Output (Transmitter ID 56)
The package is a 11 bit, 16 Channel Package, with the same information as the sbus package.

## What are all these different structs?
 * `state`: The current state of the airplane as measured by the BNO055
 * `out_state`: The (desired) state of the outputs (servos and motors)
 * `setpoint`: The setpoint sent by the flightcomputer (power, pitch, roll)
