'''
Created on Apr 25, 2015

@author: iclosure
'''
from PyQt5.QtSerialPort import QSerialPort
from PyQt5.Qt import QObject, QDateTime, QSettings, QIODevice
from PyQt5 import QtCore
from com.smartsoft.iclosure.protocol.protocol import SerialRecv, SerialSend, \
    SerialPortConfig
import ctypes
import struct

class SerialPortProxy(QObject):
    '''
    class SerialPortProxy
    '''
    stateChanged = QtCore.pyqtSignal(str)
    serialPortError = QtCore.pyqtSignal(QSerialPort.SerialPortError, str)
    displayRespond = QtCore.pyqtSignal(SerialRecv, QDateTime)
    def __init__(self, parent = None):
        super(SerialPortProxy, self).__init__(parent)
        self.setObjectName('SerialPortProxy')

        #
        self._currIndex = 0
        self._currFrameId = 0
        self._frameSize = 0
        self._streamBuff = []
        self._newBuff = [0] * 4096
        self._pkgBuff = [0] * 100
        self._serialRecv = SerialRecv()

        self._serialConfig = SerialPortConfig()
        self._serialPort = QSerialPort(self)

        #
        self._serialPort.error.connect(self.onSerialPortError)
        self._serialPort.readyRead.connect(self.readData)

        #
        self._serialSimulate = None  # SerialSimulate(self)

        # read configuration
        settings = QSettings(self)
        # group - serialport properties
        settings.beginGroup('Settings/' + self.objectName() + '/SerialPort')
        self._serialConfig.port = settings.value('port', 'COM1')
        infolist = settings.value('info', '115200-8-N-1').split('-')
        self._serialConfig.baudRate = int(infolist[0])
        dataBits = int(infolist[1])
        self._serialConfig.dataBits = (QSerialPort.Data5 if dataBits == 5
                                       else QSerialPort.Data6 if dataBits == 6
                                       else QSerialPort.Data7 if dataBits == 7
                                       else QSerialPort.Data8 if dataBits == 8
                                       else QSerialPort.Data8)
        parity = infolist[2][0].upper()
        self._serialConfig.parity = (QSerialPort.NoParity if parity == 'N'
                                     else QSerialPort.EvenParity if parity == 'E'
                                     else QSerialPort.OddParity if parity == 'O'
                                     else QSerialPort.SpaceParity if parity == 'S'
                                     else QSerialPort.MarkParity if parity == 'M'
                                     else QSerialPort.NoParity)
        stopBits = int(float(infolist[3]) * 10)
        self._serialConfig.stopBits = (QSerialPort.OneStop if stopBits == 10
                                       else QSerialPort.OneAndHalfStop if stopBits == 15
                                       else QSerialPort.TwoStop if stopBits == 20
                                       else QSerialPort.OneStop)
        settings.endGroup()

    def config(self):
        return  self._serialConfig

    def setConfig(self, info):
        self._serialConfig.port = info.port
        self._serialConfig.baudRate = info.baudRate
        self._serialConfig.dataBits = info.dataBits
        self._serialConfig.parity = info.parity
        self._serialConfig.stopBits = info.stopBits

        #
        settings = QSettings(self)
        settings.beginGroup('Settings/' + self.objectName() + '/SerialPort')
        settings.setValue('port', self._serialConfig.port)
        settings.setValue('info', self.__str__())
        settings.endGroup()

    @QtCore.pyqtSlot(QSerialPort.SerialPortError, str)
    def onSerialPortError(self, error):
        print('SerialPort open failed!, %d' % error)
        self.serialPortError.emit(error,
                                  'No error' if error == QSerialPort.NoError
                                  else 'Device not found' if error == QSerialPort.DeviceNotFoundError
                                  else 'Permission error' if error == QSerialPort.PermissionError
                                  else 'Open error' if error == QSerialPort.OpenError
                                  else 'Parity error' if error == QSerialPort.ParityError
                                  else 'Framing error' if error == QSerialPort.FramingError
                                  else 'Break Condition error' if error == QSerialPort.BreakConditionError
                                  else 'Write error' if error == QSerialPort.WriteError
                                  else 'Read error' if error == QSerialPort.ReadError
                                  else 'Resource error' if error == QSerialPort.ResourceError
                                  else 'Unsupported operation' if error == QSerialPort.UnsupportedOperationError
                                  else 'Unknown error' if error == QSerialPort.UnknownError
                                  else 'Timeout' if error == QSerialPort.TimeoutError
                                  else 'Not open' if error == QSerialPort.NotOpenError
                                  else '%d' % error)

    @QtCore.pyqtSlot()
    def start(self):
        if self._serialPort.isOpen():
            self._serialPort.close()

        # config serialport properties
        self._serialPort.setPortName(self._serialConfig.port)
        if self._serialPort.open(QIODevice.ReadWrite):
            self._serialPort.setBaudRate(self._serialConfig.baudRate)
            self._serialPort.setDataBits(self._serialConfig.dataBits)
            self._serialPort.setParity(self._serialConfig.parity)
            self._serialPort.setStopBits(self._serialConfig.stopBits)
            self.stateChanged.emit('Open')

    @QtCore.pyqtSlot()
    def stop(self):
        if self._serialPort.isOpen():
            self._serialPort.close()
            self.stateChanged.emit('Close')

    @QtCore.pyqtSlot()
    def save(self):
        self.setProperty('portState', self._serialPort.isOpen())
        self.stop()
        # save state of simulator
        if self._serialSimulate:
            self._serialSimulate.save()

    @QtCore.pyqtSlot()
    def restore(self):
        self.start() if bool(self.property('portState')) else self.stop()

        # restore state of simulator
        if self._serialSimulate:
            self._serialSimulate.restore()

    @QtCore.pyqtSlot(SerialSend)
    def writeData(self, data):
        if not self._serialPort.isOpen():
            # warning...
            return 0
        return int(self._serialPort.write(data.pack()))

    @QtCore.pyqtSlot()
    def readData(self):
        self._streamBuff = self._serialPort.read(4096)
        self.unpack()
        return self._streamBuff.__len__()

    def unpack(self):
        for (i, value) in enumerate(self._streamBuff):
            if self._currIndex < self._serialRecv._offset_length:  # 1.frame-header
                self._frameSize = 0
                if value == self._serialRecv._headers[self._currIndex]:
                    self._newBuff[self._currIndex] = value
                    self._currIndex += 1
                else:
                    self._currIndex = 0
                continue
            elif self._currIndex == self._serialRecv._offset_length:  # 2.frame-length
                if value == self._serialRecv.length:
                    pass
                else:
                    self._currIndex = 0
                    continue
                self._frameSize = value
                self._newBuff[i] = value
                self._currIndex += 1
                continue
            elif self._currIndex == self._serialRecv._offset_index:  # 3.frame-index
                self._currFrameId = value
                self._newBuff[i] = value
                self._currIndex += 1
                continue
            elif self._currIndex < self._frameSize:  # 4.frame-data
                self._newBuff[i] = value
                self._currIndex += 1
            if self._currIndex == self._frameSize:  # receive a full frame successfully
                # 5.frame-sum
                s = 0
                for index in range(0, self._serialRecv._offset_sum):
                    s = (s + self._newBuff[index]) & 0xff
                if self._newBuff[ self._serialRecv._offset_sum] != s:
                    self._currIndex = 0
                    # continue  # invalid frame
                # 6.frame-tail
                if self._newBuff[self._serialRecv._offset_tail] != int(self._serialRecv.tail):
                    self._currIndex = 0  # invalid frame
                    continue
                # 7.save as...
                self._pkgBuff = self._newBuff[0:self._frameSize]
                # 8.dispatch
                self.dispatch()
                # 9.reset
                self._currIndex = 0

    def dispatch(self):
        # save as...
        self._serialRecv.unpack(bytes(self._pkgBuff))

        # convert

        self._serialRecv.lMBrakeP = self.swapUint16(self._serialRecv.lMBrakeP)
        self._serialRecv.lABrakeP = self.swapUint16(self._serialRecv.lABrakeP)
        self._serialRecv.rMBrakeP = self.swapUint16(self._serialRecv.rMBrakeP)
        self._serialRecv.rABrakeP = self.swapUint16(self._serialRecv.rABrakeP)

        self._serialRecv.lMRotateP = self.swapUint16(self._serialRecv.lMRotateP)
        self._serialRecv.lARotateP = self.swapUint16(self._serialRecv.lARotateP)
        self._serialRecv.rMRotateP = self.swapUint16(self._serialRecv.rMRotateP)
        self._serialRecv.rARotateP = self.swapUint16(self._serialRecv.rARotateP)

        self._serialRecv.lWheelSpd = self.swapUint32(self._serialRecv.lWheelSpd)
        self._serialRecv.rWheelSpd = self.swapUint32(self._serialRecv.rWheelSpd)

        # emit
        self.displayRespond.emit(self._serialRecv, QDateTime.currentDateTime())

    _crc16_table = bytes(256)
    @staticmethod
    def calcRCR16(data):
        cnta = 0
        cntb = 0
        crcval = 0

    @staticmethod
    def serialPortSendSum(data):
        return 0  # TODO
        s = 0
        for i in range(0, data._offset_sum):
            s = (s + data[i]) & 0xff
        return s

    @staticmethod
    def serialPortRecvSum(data):
        return 0  # TODO
        s = 0
        for i in range(0, data._offset_sum):
            s = (s + data[i]) & 0xff
        return s

    @staticmethod
    def swapUint16(value):
        return value

    @staticmethod
    def swapUint32(value):
        return value

    @staticmethod
    def convertFromASCII(value):
        return value

    @staticmethod
    def convertToASCII(value):
        return value
