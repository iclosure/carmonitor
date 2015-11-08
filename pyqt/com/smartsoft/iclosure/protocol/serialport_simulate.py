'''
Created on Apr 25, 2015

@author: iclosure
'''
from PyQt5 import QtCore
from PyQt5.QtCore import Qt
from PyQt5.QtSerialPort import QSerialPort
from PyQt5.Qt import QObject, QDateTime, QIODevice
from com.smartsoft.iclosure.protocol.protocol import SerialSend, SerialRecv,\
    SerialPortConfig
from com.smartsoft.iclosure.protocol.serialport_proxy import SerialPortProxy
import ctypes
import random
#import random

class SerialSimulate(QObject):
    '''
    class SerialSimulate
    '''
    stateChanged = QtCore.pyqtSignal(str)
    serialPortError = QtCore.pyqtSignal(QSerialPort.SerialPortError, str)
    def __init__(self, parent=None):
        super(SerialSimulate, self).__init__(parent)
        self.setObjectName('SerialSimulate')
        #
        self._currIndex = 0
        self._currFrameId = 0
        self._frameSize = 0
        self._streamBuff = []
        self._newBuff = [0] * 4096
        self._pkgBuff = [0] * 100
        self._serialSend = SerialSend()
        self._serialRecv = SerialRecv()
        
        self._serialConfig = SerialPortConfig()
        self._serialPort = QSerialPort(self)
        
        #
        self._serialPort.error.connect(self.onSerialPortError)
        self._serialPort.readyRead.connect(self.readData)

        #
        self._serialPort.setPortName('COM5')
        if self._serialPort.open(QIODevice.ReadWrite):
            self._serialPort.setBaudRate(QSerialPort.Baud115200)
            self._serialPort.setDataBits(QSerialPort.Data8)
            self._serialPort.setParity(QSerialPort.OddParity)
            self._serialPort.setStopBits(QSerialPort.OneStop)
            self.stateChanged.emit('Open')
        
        #
        self._serialRecv.lMBrakeP = SerialPortProxy.swapUint16(0)
        self._serialRecv.rMBrakeP = SerialPortProxy.swapUint16(0)
        self._serialRecv.lABrakeP = SerialPortProxy.swapUint16(5)
        self._serialRecv.rABrakeP = SerialPortProxy.swapUint16(5)
        self._serialRecv.lMRotateP = SerialPortProxy.swapUint16(0)
        self._serialRecv.rMRotateP = SerialPortProxy.swapUint16(0)
        self._serialRecv.lARotateP = SerialPortProxy.swapUint16(5)
        self._serialRecv.rARotateP = SerialPortProxy.swapUint16(5)
        self._serialRecv.lWheelSpd = self.convertToASCII(100)
        self._serialRecv.rWheelSpd = self.convertToASCII(100)

        #
        self._timerId = self.startTimer(100, Qt.PreciseTimer)

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
    
    @QtCore.pyqtSlot()
    def restore(self):
        self.start() if bool(self.property('portState')) else self.stop()
    
    @QtCore.pyqtSlot(SerialRecv)
    def writeData(self, data):
        if not self._serialPort.isOpen():
            # warning...
            self.killTimer(self._timerId)
            return 0
        return int(self._serialPort.write(data.pack()))
    
    @QtCore.pyqtSlot()
    def readData(self):
        self._streamBuff = self._serialPort.read(4096)
        self.unpack()
        return self._streamBuff.__len__()
    
    def unpack(self):
        for (i, value) in enumerate(self._streamBuff):
            if self._currIndex < self._serialSend._offset_length:  # 1.frame-header
                self._frameSize = 0
                if value == self._serialSend._headers[self._currIndex]:
                    self._newBuff[self._currIndex] = value
                    self._currIndex += 1
                else:
                    self._currIndex = 0
                continue
            elif self._currIndex == self._serialSend._offset_length:  # 2.frame-length
                if value == self._serialSend.length:
                    pass
                else:
                    self._currIndex = 0
                    continue
                self._frameSize = value
                self._newBuff[i] = value
                self._currIndex += 1
                continue
            elif self._currIndex == self._serialSend._offset_index:   # 3.frame-index
                self._currFrameId = value
                self._newBuff[i] = value
                self._currIndex += 1
                continue
            elif self._currIndex < self._frameSize:                   # 4.frame-data
                self._newBuff[i] = value
                self._currIndex += 1
            if self._currIndex == self._frameSize:                    # receive a full frame successfully
                # 5.frame-sum
                s = 0
                for index in range(0, self._serialSend._offset_sum):
                    s = (s + self._newBuff[index]) & 0xff
                if self._newBuff[self._serialSend._offset_sum] != s:
                    self._currIndex = 0
                    #continue  # invalid frame
                # 6.frame-tail
                if self._newBuff[self._serialSend._offset_tail] != int(self._serialSend.tail):
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
        self._serialSend.unpack(bytes(self._pkgBuff))
        
        # send
        self._serialRecv.lWheelSpd = self.convertToASCII(int(self._serialSend.lWheelSpd / 42.94967296))
        self._serialRecv.rWheelSpd = self.convertToASCII(int(self._serialSend.rWheelSpd / 42.94967296))
        self._serialRecv.sum = SerialPortProxy.serialPortRecvSum(self._serialRecv)
        self.writeData(self._serialRecv)
        
    def timerEvent(self, event):
        step = 1
        if event.timerId() == self._timerId:
            self._serialRecv.index += 1
            if self._serialSend.ctrlWord.lPowerSw:
                self._serialRecv.lMBrakeP = self.stepInc(self._serialRecv.lMBrakeP, step, 30)
                self._serialRecv.lABrakeP = self.stepInc(self._serialRecv.lABrakeP, step, 30)
                self._serialRecv.lMRotateP = self.stepInc(self._serialRecv.lMRotateP, step, 30)
                self._serialRecv.lARotateP = self.stepInc(self._serialRecv.lARotateP, step, 30)
                self._serialRecv.lWheelSpd = self.stepInc(self._serialRecv.lWheelSpd, step, 3000)
            if self._serialSend.ctrlWord.rPowerSw:
                self._serialRecv.rMBrakeP = self.stepInc(self._serialRecv.rMBrakeP, step, 30)
                self._serialRecv.rABrakeP = self.stepInc(self._serialRecv.rABrakeP, step, 30)
                self._serialRecv.rMRotateP = self.stepInc(self._serialRecv.rMRotateP, step, 30)
                self._serialRecv.rARotateP = self.stepInc(self._serialRecv.rARotateP, step, 30)
                self._serialRecv.rWheelSpd = self.stepInc(self._serialRecv.rWheelSpd, step, 3000)
            self._serialRecv.sum = SerialPortProxy.serialPortRecvSum(self._serialRecv)
            self.writeData(self._serialRecv)
        
    @staticmethod
    def stepWheelSpd(value, step, maxValue):
        value = SerialPortProxy.convertFromASCII(value) + step
        if value > maxValue:
            value = 0
        return SerialSimulate.convertToASCII(value)

    @staticmethod
    def convertToASCII(value):
        return value
    
    @staticmethod
    def stepInc(value, step, maxValue):
        value = SerialPortProxy.swapUint16(value) + step# + random() % 50
        if value > maxValue:
            value = 0
        return SerialPortProxy.swapUint16(value)
    