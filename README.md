# FlightController
## Compile
```
cd cmake-build-debug && make
```
## Flash
```
cd cmake-build-debug && st-flash --reset write FlightController.bin 0x8000000
```

## Package format
### FlightController Output (Transmitter ID 23)
The output package is a 10 bit, 16 Channel Package with the following data:

| Channel | Data |
| --- | --- |
| 0 | Heading (value between 0 and 360) |
| 1 | Roll + 180 |
| 2 | Pitch + 180 |
| 3 | Ultrasonic-Height |
| 4 | Barometer-Height |
| 5 | Airspeed |
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

### PowerDistributionBoard Output (Transmitter ID 74)
The package is a 8 bit, 8 Channel Package, the information in each channel corresponds to the response
to the command with the respective channel number.