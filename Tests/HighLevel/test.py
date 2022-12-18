import Lib.port_manager as port_manager
import Lib.bno_mock as bno
import Lib.avr_dude as avr_dude
import serial

import time


def main():
    # Flash the chip
    #avr_dude.flash("FlightController.hex")
    avr_dude.reset_device()

    # Wait for the device to enter error handler failsave
    time.sleep(1)

    # Initialize all interfaces
    ports = port_manager.get_all_ports()
    print(ports)
    bno_mock = bno.Mock(ports["bno"])
    bno_mock.reg_map[0x36] = 0xF  # Self test passed

    fcp_serial = serial.Serial(port=ports["fcp"], baudrate=115200, )

    # Restart the device
    avr_dude.reset_device()

    while True:
        bno_mock.process()
        fcp_dat = fcp_serial.read(fcp_serial.in_waiting)
        if len(fcp_dat) > 0:
            print(fcp_dat)


if __name__ == "__main__":
    main()
