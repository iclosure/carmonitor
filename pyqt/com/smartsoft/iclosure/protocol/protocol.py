'''
Created on Apr 25, 2015

@author: iclosure
'''

import ctypes
from _functools import reduce
from PyQt5.QtSerialPort import QSerialPort
import struct

def c_memset(obj, value, size): ctypes.memset(id(obj) + size, value, size)

class SerialPortConfig:
    '''
    class SerialPortConfig
    '''
    def __init__(self):
        self.port = 'COM1'
        self.baudRate = QSerialPort.Baud115200
        self.dataBits = QSerialPort.Data8
        self.parity = QSerialPort.NoParity
        self.stopBits = QSerialPort.OneStop
        
    def __str__(self, prePort=True):
        text = ''
        if prePort:
            text = self.port + ': '
        text += '%d' % self.baudRate
        text += '-%d' % (5 if self.dataBits == QSerialPort.Data5
                         else 6 if self.dataBits == QSerialPort.Data6
                         else 7 if self.dataBits == QSerialPort.Data7
                         else 8 if self.dataBits == QSerialPort.Data8
                         else 0)
        text += '-%s' % ('N' if self.parity == QSerialPort.NoParity
                         else 'E' if self.parity == QSerialPort.EvenParity
                         else 'O' if self.parity == QSerialPort.OddParity
                         else 'S' if self.parity == QSerialPort.SpaceParity
                         else 'M' if self.parity == QSerialPort.MarkParity
                         else '!')
        text += '-%.1f' % (1 if self.stopBits == QSerialPort.OneStop
                           else 1.5 if self.stopBits == QSerialPort.OneAndHalfStop
                           else 2 if self.stopBits == QSerialPort.TwoStop
                           else 0)
        return text
    

class SerialSend(ctypes.Structure):
    '''
    class SerialSend
    
    info:
        size: 30 bytes
    '''
    class _CtrlWord(ctypes.Union):
        _pack_ = 1
        class _Bits(ctypes.Structure):
            _pack_ = 1
            _fields_ = [('lTracksip', ctypes.c_uint8, 1),
                        ('rTracksip', ctypes.c_uint8, 1),
                        ('lPowerSw', ctypes.c_uint8, 1),
                        ('rPowerSw', ctypes.c_uint8, 1)
                        ]
        _anonymous_ = ('bits',)
        _fields_= [('value', ctypes.c_uint8),
                    ('bits', _Bits)
                    ]
    _pack_ = 1
    _fields_ = [('header', ctypes.c_uint16),
                ('length', ctypes.c_uint8),
                ('index', ctypes.c_uint8),
                ('lWheelSpd', ctypes.c_uint32),
                ('rWheelSpd', ctypes.c_uint32),
                ('ctrlWord', _CtrlWord),
                ('reserved', ctypes.c_uint8 * 15),
                ('sum', ctypes.c_uint8),
                ('tail', ctypes.c_uint8),
                ]
    _format_ = '<H2B2IB15x2B'
    def __init__(self):
        self.header = 0x55AA
        self.length = ctypes.sizeof(SerialSend)
        self.tail = 0x9F
        #
        self._headers = struct.pack('H', self.header)
        self._offset_length = 2
        self._offset_index = self._offset_length + 1
        self._offset_tail = self.length - 1
        self._offset_sum = self._offset_tail - 1
        
    def pack(self):
        return struct.pack(self._format_,
                           self.header, self.length, self.index,
                           self.lWheelSpd, self.rWheelSpd,
                           self.ctrlWord.value,
                           self.sum, self.tail)
    
    def unpack(self, buffer):
        (self.header, self.length, self.index,
          self.lWheelSpd, self.rWheelSpd,
          self.ctrlWord.value,
          self.sum, self.tail) = struct.unpack(self._format_, buffer)
        
    def __str__(self):
        return (u'{0:04X} {1:02X} {2:02X} '
                '{3:08X} {4:08X} '
                '{5:02X} '
                '{6} '
                '{7:02X} {8:02X}'
                .format(self.header, self.length, self.index,
                        self.lWheelSpd, self.rWheelSpd,
                        self.ctrlWord.value,
                        reduce((lambda x, y: x + y), ['%02X ' % v for v in tuple(self.reserved)]),
                        self.sum, self.tail)
                )
    
class SerialRecv(ctypes.Structure):
    '''
    class SerialRecv
    
    info:
        size: 36 bytes
    '''
    _pack_ = 1
    _fields_ = [('header', ctypes.c_uint16),
                ('length', ctypes.c_uint8),
                ('index', ctypes.c_uint8),
                
                ('lMBrakeP', ctypes.c_uint16),    # left-major-brake-press
                ('lABrakeP', ctypes.c_uint16),    # left-minor-brake-press
                ('rMBrakeP', ctypes.c_uint16),    # right-major-brake-press
                ('rABrakeP', ctypes.c_uint16),    # right-minor-brake-press
                
                ('lMRotateP', ctypes.c_uint16),   # left-major-rotate-rate
                ('lARotateP', ctypes.c_uint16),   # left-minor-rotate-rate
                ('rMRotateP', ctypes.c_uint16),   # right-major-rotate-rate
                ('rARotateP', ctypes.c_uint16),   # right-minor-rotate-rate
                
                ('rWheelSpd', ctypes.c_uint32),   # right-wheel-speed
                ('lWheelSpd', ctypes.c_uint32),   # left-wheel-speed
                
                ('reserved', ctypes.c_uint8 * 6), #
                ('sum', ctypes.c_uint8),          # parity sum
                ('tail', ctypes.c_uint8),
                ]
    _format_ = '<H2B8H2I6x2B'
    def __init__(self):
        self.header = 0x55AA
        self.length = ctypes.sizeof(SerialRecv)
        self.tail = 0x9F
        #
        self._headers = struct.pack('H', self.header)
        self._offset_length = 2
        self._offset_index = self._offset_length + 1
        self._offset_tail = self.length - 1
        self._offset_sum = self._offset_tail - 1
        
    def pack(self):
        return struct.pack(self._format_,
                           self.header, self.length, self.index,
                           self.lMBrakeP, self.lABrakeP, self.rMBrakeP, self.rABrakeP,
                           self.lMRotateP, self.lARotateP, self.rMRotateP, self.rARotateP,
                           self.rWheelSpd, self.lWheelSpd,
                           self.sum, self.tail)
    
    def unpack(self, buffer):
        (self.header, self.length, self.index,
         self.lMBrakeP, self.lABrakeP, self.rMBrakeP, self.rABrakeP,
         self.lMRotateP, self.lARotateP, self.rMRotateP, self.rARotateP,
         self.rWheelSpd, self.lWheelSpd,
         self.sum, self.tail) = struct.unpack(self._format_, buffer)
        
    def __str__(self):
        return (u'{0:04X} {1:02X} {2:02X} '
                '{3:04X} {4:04X} {5:04X} {6:04X} '
                '{7:04X} {8:04X} {9:04X} {10:04X} '
                '{11:08X} {12:08X} '
                '{13} '
                '{14:02X} {15:02X}'
                .format(self.header, self.length, self.index,
                        self.lMBrakeP, self.lABrakeP, self.rMBrakeP, self.rABrakeP,
                        self.lMRotateP, self.lARotateP, self.rMRotateP, self.rARotateP,
                        self.lWheelSpd, self.rWheelSpd,
                        reduce((lambda x, y: x + y), ['%02X ' % v for v in tuple(self.reserved)]),
                        self.sum, self.tail)
                )
    
#TEST
if __name__ == '__main__':
    strSend = b'\xaaU\x1f\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x9f'
    strRecv = b'\xaaU$\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x9f'
    serialSend = SerialSend()
    serialRecv = SerialRecv()
    print('SerianSend::length: %d' % serialSend.length)
    print('SerianRecv::length: %d' % serialRecv.length)
    print('serialSend', serialSend)
    print('serialRecv: ', serialRecv)
    print('serialSend.pack:', serialSend.pack())
    print('serialRecv.pack:', serialRecv.pack())
    a = serialSend.unpack(strSend)
    print(serialSend.pack())
    a = serialRecv.unpack(strRecv)
    print(serialRecv.pack())
    