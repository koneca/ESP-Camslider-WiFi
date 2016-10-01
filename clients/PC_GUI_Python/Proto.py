
import binascii

FORWARD = 1
BACKWARD = 2
STOP = 3
RUN = 4
SHOOT = 5


class Message:
    def __init__(self):
        self.type = 0
        self.length = 0
        self.value = ""

    @property
    def Serialize(self):
        return bytearray([self.type, self.length, ]) + bytearray(self.value)
