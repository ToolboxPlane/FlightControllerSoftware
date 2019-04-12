# FlightController
## Building and deploy
### Compile
To compile the firmware run:
```
cmake . && make
```
To compile the tests run the same commands in the test directory 
#### WTF? Two independet cmake-projects?
You may ask why is there a second, independet cmake project for the tests (and you have the best right to do so). 
Well i do not like the solution either but it seems like cmake is not able to have two targets with different compilers. 
So if you find a better solution for doing this (no i won't use Makefiles or Bazel or Scons or Gradle or whatever...) please write an issue and i will fix this immediatly.

### Flash
```
TODO
```

### Fuses
```
TODO
```

#### Flash
```
TODO
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
