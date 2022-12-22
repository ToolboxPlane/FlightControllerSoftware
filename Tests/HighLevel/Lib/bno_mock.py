import serial
import enum


class RequestType(enum.Enum):
    WRITE = 0,
    READ = 1


class Request:
    def __init__(self):
        self.type = None
        self.addr = None
        self.length = None
        self.data = None


class State(enum.Enum):
    INITIAL = 0,
    IDLE = 1,
    START_RECEIVED = 2,
    RW_RECEIVED = 3,
    ADDR_RECEIVED = 4,
    IN_DATA = 5


class Mock:

    def __init__(self, port: str) -> None:
        self.serial = serial.Serial(port=port, baudrate=115200)
        self._state = State.INITIAL
        self._request = Request()
        self.reg_map = dict()

    def _send_response(self):
        if self._request.type == RequestType.WRITE:
            print(f"[BNO] writing {len(self._request.data)} bytes to {self._request.addr:02X}")
            for i, datum in enumerate(self._request.data):
                self.reg_map[self._request.addr + i] = datum
            reply = bytes([0xEE, 0x01])
        elif self._request.type == RequestType.READ:
            print(f"[BNO] reading {self._request.length} bytes from {self._request.addr:02X}")
            reply = bytearray([0xBB, self._request.length])
            for i in range(self._request.length):
                addr = self._request.addr + i
                if addr in self.reg_map:
                    reply.append(self.reg_map[addr])
                else:
                    reply.append(0)
        else:
            raise Exception(f"Invalid request {self._request.type}!")

        self._request = Request()
        self.serial.write(reply)

    def process(self):
        data = self.serial.read(self.serial.in_waiting)
        for datum in data:
            if self._state == State.INITIAL:
                if datum == 0xAA:
                    self._state = State.START_RECEIVED
            elif self._state == State.IDLE:
                if datum == 0xAA:
                    self._state = State.START_RECEIVED
                else:
                    raise Exception(f"Invalid start byte {datum}")
            elif self._state == State.START_RECEIVED:
                if datum == 0x00 or datum == 0x01:
                    self._state = State.RW_RECEIVED
                    if datum == 0x00:
                        self._request.type = RequestType.WRITE
                    elif datum == 0x01:
                        self._request.type = RequestType.READ
                    else:
                        raise Exception(f"Invalid request {datum}!")
                else:
                    raise Exception(f"Invalid read/write byte {datum}")
            elif self._state == State.RW_RECEIVED:
                self._request.addr = datum
                self._state = State.ADDR_RECEIVED
            elif self._state == State.ADDR_RECEIVED:
                self._request.length = datum
                if self._request.type == RequestType.WRITE:
                    if self._request.length == 0:
                        raise Exception("Invalid write with length 0!")
                    self._state = State.IN_DATA
                    self._request.data = []
                else:
                    self._send_response()
                    self._state = State.IDLE
            elif self._state == State.IN_DATA:
                self._request.data.append(datum)
                self._request.length -= 1
                if self._request.length == 0:
                    self._send_response()
                    self._state = State.IDLE
            else:
                raise Exception("Invalid state!")
