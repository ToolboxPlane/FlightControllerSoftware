#!/usr/bin/env bash
scp FlightController.bin pi@192.168.42.1:~
ssh pi@192.168.42.1 ./stlink/build/st-flash write FlightController.bin 0x8000000