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
| 5 | BNO-055 Calibration Status |
| 6 | Empty |
| 7 | ... |

### SBus Output (Transmitter ID 56)
The package is a 11 bit, 16 Channel Package, with the same information as the sbus packagethe same information as the sbus package.

### PowerDistributionBoard Output (Transmitter ID 74)
The package is a 8 bit, 8 Channel Package, the information in each channel corresponds to the response
to the command with the respective channel number.