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
The output package is a 10bit, 16Channel Package with the following data:

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