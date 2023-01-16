# FlightController

* [Documentation](https://toolboxplane.github.io/FlightControllerSoftware/doxygen)
* [Test Reports](https://toolboxplane.github.io/FlightControllerSoftware/report/)
* [Coverage Reports](https://toolboxplane.github.io/FlightControllerSoftware/coverage/)

## Building and flashing

### Build

To compile the firmware run:

```bash
cmake -B build
cmake --build build --target FlightController.hex
```

### How to flash to firmware

This assumes that an AVRISP mk2 is used, if you use a different programmer
change the ```-c``` flag.

```bash
avrdude -p m2560 -c avrisp2 -U flash:w:FlightController.hex:i
```

### Fuses

The fuses select the "Ext. Crystal Osc.; Frequency 8.0- MHz; Start-up time: 16K CK + 4.1 ms; [CKSEL=1111 SUT=10]"
clock, disables the clock divider, enables the preserve eeprom flag, enables the watchdog timer, enables SPI and
disables
JTAG and OCD. The brownout detection level is set to 1.8V.

| Fusebyte | Value |
|----------|-------|
| Low      | 0xEF  |
| High     | 0xC1  |
| Extended | 0xFE  |

#### How to flash the fuses

```bash
avrdude -p m2560 -B 10 -c avrisp2 -U lfuse:w:0xef:m -U hfuse:w:0xc1:m -U efuse:w:0xfe:m
```

## LEDs

The LEDs are controlled by the error handler. The lower 4 LEDs signal the
component which triggered the error handler. The upper 4 LEDs signal the exception
ID.

The error IDs are:

| Component      | Exception                | LED configuration |
|----------------|--------------------------|-------------------|
| Application    | -                        | xxxx ◯◯◯🔴        |
| System         | Timer Runtime            | ◯◯◯🔴 ◯◯🔴◯       |
|                | Watchdog                 | ◯◯🔴◯ ◯◯🔴◯       |
|                | Brownout                 | ◯◯🔴🔴 ◯◯🔴◯      |
| IMU            | Init timeout error       | ◯◯◯🔴 ◯◯🔴🔴      |
|                | Init self test error     | ◯◯🔴◯ ◯◯🔴🔴      |
|                | Status error             | ◯◯🔴🔴 ◯◯🔴🔴     |
|                | Read timeout             | ◯🔴◯◯ ◯◯🔴🔴      |
| Flightcomputer | -                        | xxxx ◯🔴◯◯        |
| Remote         | -                        | xxxx ◯🔴◯🔴       |
| Mode Handler   | No IMU Data              | ◯◯◯🔴 ◯🔴🔴◯      |
|                | No FCP Data              | ◯◯🔴◯ ◯🔴🔴◯      |
|                | No Remote Data           | ◯◯🔴🔴 ◯🔴🔴◯     |
| BNO055         | Unexpected read success  | ◯◯◯🔴 ◯🔴🔴🔴     |
|                | Unexpected write success | ◯◯🔴◯ ◯🔴🔴🔴     |
|                | Read fail                | ◯◯🔴🔴 ◯🔴🔴🔴    |
|                | Write fail               | ◯🔴◯◯ ◯🔴🔴🔴     |
|                | Invalid address          | ◯🔴◯🔴 ◯🔴🔴🔴    |
|                | Write disabled           | ◯🔴🔴◯ ◯🔴🔴🔴    |
|                | Wrong start byte         | ◯🔴🔴🔴 ◯🔴🔴🔴   |
|                | Bus overrun              | 🔴◯◯◯ ◯🔴🔴🔴     |
|                | Max length               | 🔴◯◯🔴 ◯🔴🔴🔴    |
|                | Min length               | 🔴◯🔴◯ ◯🔴🔴🔴    |
|                | Receive char timeout     | 🔴◯🔴🔴 ◯🔴🔴🔴   |
|                | Callback buffer invalid  | 🔴🔴◯◯ ◯🔴🔴🔴    |
|                | Invalid sync             | 🔴🔴◯🔴 ◯🔴🔴🔴   |

