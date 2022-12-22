import serial.tools.list_ports

FCP_USB_ID = "1A86:7523"
BNO_USB_ID = "0403:6001"


def get_port(usb_id):
    device_list = serial.tools.list_ports.comports()
    for device in device_list:
        if device.vid is not None or device.pid is not None:
            test_id = f"{device.vid:04X}:{device.pid:04x}"
            if usb_id == test_id:
                return device.device
    return None

def get_all_ports():
    return {
        "fcp": get_port(FCP_USB_ID),
        "bno": get_port(BNO_USB_ID)
    }
