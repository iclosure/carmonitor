'''
Created on Apr 5, 2015

@author: iclosure
'''

from PyQt5 import QtCore
from PyQt5.QtCore import Qt, QRect
from PyQt5.Qt import QWidget, QIcon, qApp, QGroupBox, QPushButton, QHBoxLayout, \
    QPixmap, QPainter, QVBoxLayout, QLabel, QGridLayout, QLineEdit, QMessageBox, \
    QSerialPort, QPoint, QDateTime
import ctypes
import resource.image.res_image_rc as res_image_rc
from com.smartsoft.iclosure.curve_widget import CurveWidget
from com.smartsoft.iclosure.slider_widget import SliderWidget
from com.smartsoft.iclosure.protocol.protocol import SerialSend, c_memset
from com.smartsoft.iclosure.protocol.serialport_proxy import SerialPortProxy
from com.smartsoft.iclosure.database.database_mgr import DatabaseMgr
from com.smartsoft.utils.utils import applicationDirPath
from com.smartsoft.iclosure.protocol.serialport_simulate import SerialSimulate
from com.smartsoft.iclosure.settings_widget import SettingsWidget
from com.smartsoft.iclosure.history_widget import HistoryWidget

class JSwitchButton(QPushButton):
    '''
    class JSwitchButton
    '''
    stateChanged = QtCore.pyqtSignal(bool)
    def __init__(self, pixmap = None, parent = None):
        super(JSwitchButton, self).__init__(parent)
        self.setObjectName('JSwitchButton')
        self.setCheckable(True)
        self.setChecked(False)
        self.isOn = False
        if pixmap == None:
            self.pixmap = QPixmap(':/carmonitor/image/button-switch.png')
        else:
            self.pixmap = pixmap
        self.setFixedSize(self.pixmap.width(), self.pixmap.height() / 2)
        self.clicked.connect(lambda : self.setState(not self.isOn))

    @QtCore.pyqtSlot()
    def setState(self, state):
        if self.isOn != state:
            self.isOn = state
            self.setChecked(self.isOn)
            self.repaint()
            self.stateChanged.emit(self.isOn)

    def setSize(self, w, h):
        self.resize(w, h)
        self.repaint()

    def paintEvent(self, e):
        painter = QPainter(self)
        h = self.pixmap.height() / 2
        painter.drawPixmap(self.rect(), self.pixmap,
                           QRect(0, h * (1 if self.isOn else 0),
                                 self.pixmap.width(), h))

class JTracksipButton(QPushButton):
    '''
    class JTracksipButton
    '''
    stateChanged = QtCore.pyqtSignal(bool)
    def __init__(self, pixmap = None, parent = None):
        super(JTracksipButton, self).__init__(parent)
        self.setObjectName('JTracksipButton')
        self.setCheckable(True)
        self.setChecked(False)
        self.isOn = False
        if pixmap == None:
            self.pixmap = QPixmap(':/carmonitor/image/button-switch.png')
        else:
            self.pixmap = pixmap
        self.setFixedSize(self.pixmap.width(), self.pixmap.height() / 2)
        self.pressed.connect(lambda : self.setState(True))
        self.released.connect(lambda : self.setState(False))

    @QtCore.pyqtSlot()
    def setState(self, state):
        if self.isOn != state:
            self.isOn = state
            self.setChecked(self.isOn)
            self.repaint()
            self.stateChanged.emit(self.isOn)

    def setSize(self, w, h):
        self.resize(w, h)
        self.repaint()

    def paintEvent(self, e):
        painter = QPainter(self)
        h = self.pixmap.height() / 2
        painter.drawPixmap(self.rect(), self.pixmap,
                           QRect(0, h * (1 if self.isOn else 0),
                                 self.pixmap.width(), h))

class JDashButton(QPushButton):
    '''
    class JDashButton
    '''
    def __init__(self, text = None, parent = None):
        super(JDashButton, self).__init__(text, parent)
        self.setObjectName('JDashButton')

class MainWidget(QWidget):
    '''
    class MainWidget
    '''
    def __init__(self, parent = None):
        super(MainWidget, self).__init__(parent)

        # member variables
        self._lTheorySpd = 0
        self._rTheorySpd = 0
        self._serialSend = SerialSend()

        # mainWindow properties
        self.setObjectName('MainWidget')
        self.setWindowIcon(QIcon(':/image/default/app.icon'))
        self.setWindowTitle('%s V%s' % (
                            qApp.applicationDisplayName(),
                            qApp.applicationVersion()))
        self.resize(800, 480)

        # mainWindow layout

        # top

        self.groupBoxTop = QGroupBox(self)
        self.groupBoxTop.setObjectName('groupBoxTop')

        # command dashboard
        buttonLeftPower = JSwitchButton(parent = self.groupBoxTop)
        buttonRightPower = JSwitchButton(parent = self.groupBoxTop)
        buttonSettings = QPushButton(self.groupBoxTop)
        buttonHistory = QPushButton(self.groupBoxTop)
        buttonQuit = QPushButton(self.groupBoxTop)

        buttonLeftPower.setObjectName('buttonLeftPower')
        buttonRightPower.setObjectName('buttonRightPower')
        buttonSettings.setObjectName('buttonSettings')
        buttonHistory.setObjectName('buttonHistory')
        buttonQuit.setObjectName('buttonQuit')

        areaPortState = QWidget(self)
        areaPortState.setObjectName('areaPortState')
        areaPortState.setStyleSheet('QWidget#areaPortState{border-radius:3px;'
                                    'border:1px solid #505050;'
                                    'background-color:rgba(64,64,64,50);}')
        vertLayoutPortState = QVBoxLayout(areaPortState)
        vertLayoutPortState.setContentsMargins(50, 2, 50, 2)
        vertLayoutPortState.setSpacing(3)

        buttonPortState = JSwitchButton(pixmap = QPixmap(':/carmonitor/image/button-port-state.png'), parent = areaPortState)
        buttonPortState.setObjectName('buttonPortState')
        vertLayoutPortState.addWidget(QLabel('串口', areaPortState), 0, Qt.AlignHCenter)
        vertLayoutPortState.addWidget(buttonPortState)

        #
        horiLayoutTop = QHBoxLayout(self.groupBoxTop)
        horiLayoutTop.setContentsMargins(0, 0, 0, 0)
        horiLayoutTop.addSpacing(25)
        horiLayoutTop.addWidget(buttonSettings, 0, Qt.AlignLeft)
        horiLayoutTop.addSpacing(20)
        horiLayoutTop.addWidget(buttonHistory, 0, Qt.AlignLeft)
        horiLayoutTop.addSpacing(65)
        horiLayoutTop.addWidget(buttonLeftPower)
        horiLayoutTop.addWidget(QLabel('左电源开关', self.groupBoxTop))
        horiLayoutTop.addStretch()
        horiLayoutTop.addWidget(areaPortState, 0, Qt.AlignTop)
        horiLayoutTop.addStretch()
        horiLayoutTop.addWidget(QLabel('右电源开关', self.groupBoxTop))
        horiLayoutTop.addWidget(buttonRightPower)
        horiLayoutTop.addSpacing(150)
        horiLayoutTop.addWidget(buttonQuit, 0, Qt.AlignRight)
        horiLayoutTop.addSpacing(25)

        # middle

        # curves
        self.curveLBP = CurveWidget(title = '左刹车压力（MPa）', parent = self)
        self.curveLRP = CurveWidget(title = '左转速（r/min）', parent = self)
        self.curveRBP = CurveWidget(title = '右刹车压力（MPa）', parent = self)
        self.curveRRP = CurveWidget(title = '右转速（r/min）', parent = self)

        self.curveLBP.setObjectName('curveLBP')
        self.curveLRP.setObjectName('curveLRP')
        self.curveRBP.setObjectName('curveRBP')
        self.curveRRP.setObjectName('curveRRP')

        # self.curveLBP.setAxisScale(QwtPlot.yLeft, 0, 30, 5)

        # areaMiddle
        self.areaMiddle = QWidget(self)
        self.areaMiddle.setObjectName('areaMiddle')
        self.areaMiddle.setFixedWidth(280)

        #
        groupBoxStatus = QGroupBox(self)
        groupBoxStatus.setObjectName('groupBoxStatus')

        # status-view
        gridLayoutStatus = QGridLayout(groupBoxStatus)
        gridLayoutStatus.setContentsMargins(5, 5, 5, 5)
        gridLayoutStatus.setHorizontalSpacing(8)
        gridLayoutStatus.setVerticalSpacing(3)

        # Brake-Command
        editLeftBrakeCmd = QLineEdit(groupBoxStatus)
        editRightBrakeCmd = QLineEdit(groupBoxStatus)
        editLeftBrakeCmd.setObjectName('editLeftBrakeCmd')
        editRightBrakeCmd.setObjectName('editRightBrakeCmd')
        editLeftBrakeCmd.setReadOnly(True)
        editRightBrakeCmd.setReadOnly(True)
        gridLayoutStatus.addWidget(QLabel('刹车指令：', groupBoxStatus),
                                   0, 0, 1, 2, Qt.AlignCenter)
        gridLayoutStatus.addWidget(editLeftBrakeCmd, 1, 0, 1, 1)
        gridLayoutStatus.addWidget(editRightBrakeCmd, 1, 1, 1, 1)

        # Major Brake Pressure
        self.editMLeftBrakeP = QLineEdit(groupBoxStatus)
        self.editMRightBrakeP = QLineEdit(groupBoxStatus)
        self.editMLeftBrakeP.setObjectName('editMLeftBrakeP')
        self.editMRightBrakeP.setObjectName('editMRightBrakeP')
        self.editMLeftBrakeP.setReadOnly(True)
        self.editMRightBrakeP.setReadOnly(True)
        gridLayoutStatus.addWidget(QLabel('主刹车压力：', groupBoxStatus),
                                   2, 0, 1, 2, Qt.AlignCenter)
        gridLayoutStatus.addWidget(self.editMLeftBrakeP, 3, 0, 1, 1)
        gridLayoutStatus.addWidget(self.editMRightBrakeP, 3, 1, 1, 1)

        # Assistant Brake Pressure
        self.editALeftBrakeP = QLineEdit(groupBoxStatus)
        self.editARightBrakeP = QLineEdit(groupBoxStatus)
        self.editALeftBrakeP.setObjectName('editALeftBrakeP')
        self.editARightBrakeP.setObjectName('editARightBrakeP')
        self.editALeftBrakeP.setReadOnly(True)
        self.editARightBrakeP.setReadOnly(True)
        gridLayoutStatus.addWidget(QLabel('副刹车压力：', groupBoxStatus),
                                   4, 0, 1, 2, Qt.AlignCenter)
        gridLayoutStatus.addWidget(self.editALeftBrakeP, 5, 0, 1, 1)
        gridLayoutStatus.addWidget(self.editARightBrakeP, 5, 1, 1, 1)

        # Rotation Rate
        self.editLeftRotateRate = QLineEdit(groupBoxStatus)
        self.editRightRotateRate = QLineEdit(groupBoxStatus)
        self.editLeftRotateRate.setObjectName('editLeftRotateRate')
        self.editRightRotateRate.setObjectName('editRightRotateRate')
        gridLayoutStatus.addWidget(QLabel('实际转速：', groupBoxStatus),
                                   6, 0, 1, 2, Qt.AlignCenter)
        gridLayoutStatus.addWidget(self.editLeftRotateRate, 7, 0, 1, 1)
        gridLayoutStatus.addWidget(self.editRightRotateRate, 7, 1, 1, 1)

        # Theory Rotation Rate
        self.editTheoryLeftRotateRate = QLineEdit(groupBoxStatus)
        self.editTheoryRightRotateRate = QLineEdit(groupBoxStatus)
        self.editTheoryLeftRotateRate.setObjectName('editTheoryLeftRotateRate')
        self.editTheoryRightRotateRate.setObjectName('editTheoryRightRotateRate')
        gridLayoutStatus.addWidget(QLabel('理论转速：', groupBoxStatus),
                                   8, 0, 1, 2, Qt.AlignCenter)
        gridLayoutStatus.addWidget(self.editTheoryLeftRotateRate, 9, 0, 1, 1)
        gridLayoutStatus.addWidget(self.editTheoryRightRotateRate, 9, 1, 1, 1)

        #
        groupBoxCtrl = QGroupBox(self)
        groupBoxCtrl.setObjectName('groupBoxCtrl')

        # status-view
        gridLayoutCtrl = QGridLayout(groupBoxCtrl)
        gridLayoutCtrl.setContentsMargins(5, 5, 5, 5)
        gridLayoutCtrl.setSpacing(20)

        # left-button
        buttonLeftDashboard = JDashButton('左指令旋钮', groupBoxCtrl)
        buttonLeftSpeedGain = JDashButton('左转速增益', groupBoxCtrl)
        buttonLeftSpeedKnob = JDashButton('左转速增益', groupBoxCtrl)
        buttonLeftTracksip = JTracksipButton(parent = groupBoxCtrl)
        buttonLeftDashboard.setObjectName('buttonLeftDashboard')
        buttonLeftSpeedGain.setObjectName('buttonLeftSpeedGain')
        buttonLeftSpeedKnob.setObjectName('buttonLeftSpeedKnob')
        buttonLeftTracksip.setObjectName('buttonLeftTracksip')
        buttonLeftTracksip.setFixedSize(110, 45)

        # right-button
        buttonRightDashboard = JDashButton('右指令旋钮', groupBoxCtrl)
        buttonRightSpeedGain = JDashButton('右转速增益', groupBoxCtrl)
        buttonRightSpeedKnob = JDashButton('右转速增益', groupBoxCtrl)
        buttonRightTracksip = JTracksipButton(parent = groupBoxCtrl)
        buttonRightDashboard.setObjectName('buttonRightDashboard')
        buttonRightSpeedGain.setObjectName('buttonRightSpeedGain')
        buttonRightSpeedKnob.setObjectName('buttonRightSpeedKnob')
        buttonRightTracksip.setObjectName('buttonRightTracksip')
        buttonRightTracksip.setFixedSize(110, 45)

        horiLayoutTracksip = QHBoxLayout()
        horiLayoutTracksip.setContentsMargins(0, 0, 0, 0)
        horiLayoutTracksip.setSpacing(5)
        horiLayoutTracksip.addWidget(buttonLeftTracksip)
        horiLayoutTracksip.addWidget(QLabel('打滑', self), 0, Qt.AlignHCenter)
        horiLayoutTracksip.addWidget(buttonRightTracksip)
        gridLayoutCtrl.addLayout(horiLayoutTracksip, 0, 0, 1, 2)

        horiLayoutDashboard = QHBoxLayout()
        horiLayoutDashboard.setContentsMargins(0, 0, 0, 0)
        horiLayoutDashboard.setSpacing(5)
        horiLayoutDashboard.addWidget(buttonLeftDashboard)
        horiLayoutDashboard.addWidget(QLabel('    ', self), 0, Qt.AlignHCenter)
        horiLayoutDashboard.addWidget(buttonRightDashboard)
        gridLayoutCtrl.addLayout(horiLayoutDashboard, 1, 0, 1, 2)

        horiLayoutSpeedGain = QHBoxLayout()
        horiLayoutSpeedGain.setContentsMargins(0, 0, 0, 0)
        horiLayoutSpeedGain.setSpacing(5)
        horiLayoutSpeedGain.addWidget(buttonLeftSpeedGain)
        horiLayoutSpeedGain.addWidget(QLabel('(粗调)', self), 0, Qt.AlignHCenter)
        horiLayoutSpeedGain.addWidget(buttonRightSpeedGain)
        gridLayoutCtrl.addLayout(horiLayoutSpeedGain, 2, 0, 1, 2)


        horiLayoutSpeedKnob = QHBoxLayout()
        horiLayoutSpeedKnob.setContentsMargins(0, 0, 0, 0)
        horiLayoutSpeedKnob.setSpacing(5)
        horiLayoutSpeedKnob.addWidget(buttonLeftSpeedKnob)
        horiLayoutSpeedKnob.addWidget(QLabel('(细调)', self), 0, Qt.AlignHCenter)
        horiLayoutSpeedKnob.addWidget(buttonRightSpeedKnob)
        gridLayoutCtrl.addLayout(horiLayoutSpeedKnob, 3, 0, 1, 2)

        #
        vertLayoutMid = QVBoxLayout(self.areaMiddle)
        vertLayoutMid.setContentsMargins(0, 0, 0, 0)
        vertLayoutMid.setSpacing(0)
        vertLayoutMid.addWidget(groupBoxStatus)
        vertLayoutMid.addWidget(groupBoxCtrl)
        vertLayoutMid.addSpacing(20)

        #
        gridLayoutBottom = QGridLayout()
        gridLayoutBottom.setContentsMargins(0, 0, 0, 0)
        gridLayoutBottom.setSpacing(1)
        gridLayoutBottom.addWidget(self.curveLBP, 0, 0, 1, 1)
        gridLayoutBottom.addWidget(self.curveLRP, 1, 0, 1, 1)
        gridLayoutBottom.addWidget(self.areaMiddle, 0, 1, 2, 1)
        gridLayoutBottom.addWidget(self.curveRBP, 0, 2, 1, 1)
        gridLayoutBottom.addWidget(self.curveRRP, 1, 2, 1, 1)

        # main-layout
        vertLayoutMain = QVBoxLayout(self)
        vertLayoutMain.setContentsMargins(5, 5, 5, 5)
        vertLayoutMain.addWidget(self.groupBoxTop)
        vertLayoutMain.addLayout(gridLayoutBottom)

        # global properties
        qApp.setProperty('MainWidget', self)
        self._serialProxy = SerialPortProxy(self)
        self._serialProxy._serialSimulate = SerialSimulate(self._serialProxy)  #
        qApp.setProperty('SerialProxy', self._serialProxy)

        #
        buttonSettings.clicked.connect(self.onButtonSettingsClicked)
        buttonHistory.clicked.connect(self.onButtonHistoryClicked)
        buttonPortState.clicked.connect(self.onButtonPortStateClicked)
        buttonQuit.clicked.connect(self.onButtonQuitClicked)

        # curves
        self.curveLBP.doubleClicked.connect(self.onCurveDoubleClicked)
        self.curveLRP.doubleClicked.connect(self.onCurveDoubleClicked)
        self.curveRBP.doubleClicked.connect(self.onCurveDoubleClicked)
        self.curveRRP.doubleClicked.connect(self.onCurveDoubleClicked)

        # switch-power
        buttonLeftPower.stateChanged.connect(self.onButtonLeftPowerStateChanged)
        buttonRightPower.stateChanged.connect(self.onButtonRightPowerStateChanged)

        # switch-tracksip
        buttonLeftTracksip.stateChanged.connect(self.onButtonLeftTracksipStateChanged)
        buttonRightTracksip.stateChanged.connect(self.onButtonRightTracksipStateChanged)

        self._serialProxy.stateChanged.connect(self.onSerialStateChanged)
        self._serialProxy.serialPortError.connect(self.onSerialPortError)
        self._serialProxy.displayRespond.connect(self.onSerialDisplayRespond)

        #
        buttonLeftSpeedGain.clicked.connect(self.execSliderWidget)
        buttonLeftSpeedKnob.clicked.connect(self.execSliderWidget)
        buttonRightSpeedGain.clicked.connect(self.execSliderWidget)
        buttonRightSpeedKnob.clicked.connect(self.execSliderWidget)

        # final initialization

        self.editMLeftBrakeP.setText('0 MPa')
        self.editMRightBrakeP.setText('0 MPa')
        self.editALeftBrakeP.setText('0 MPa')
        self.editARightBrakeP.setText('0 MPa')

        self.editLeftRotateRate.setText('0 r/min')
        self.editRightRotateRate.setText('0 r/min')
        self.editTheoryLeftRotateRate.setText('0 r/min')
        self.editTheoryRightRotateRate.setText('0 r/min')

        #
        c_memset(self._serialSend, 0, ctypes.sizeof(self._serialSend))

        # SQL
        sqlName = applicationDirPath() \
            + '/../data/cm-' \
            + QDateTime.currentDateTime().toLocalTime().toString('yyyy-MM-dd-HH-mm-ss') \
            + '.db'
        if not DatabaseMgr().create(sqlName):
            assert(False)

        # start serialport
        self._serialProxy.start()

        #
        buttonLeftTracksip.setState(self._serialSend.ctrlWord.lTracksip)
        buttonRightTracksip.setState(self._serialSend.ctrlWord.lTracksip)

    def onButtonSettingsClicked(self):
        self._serialProxy.save()
        #
        settingsWidget = SettingsWidget(self)
        settingsWidget.exec_()

        self._serialProxy.restore()

    def onButtonHistoryClicked(self):
        self._serialProxy.save()
        DatabaseMgr().save()
        #
        historyWidget = HistoryWidget(self)
        historyWidget.showMaximized()
        historyWidget.exec_()

        DatabaseMgr().restore()
        self._serialProxy.restore()

    def onButtonPortStateClicked(self, checked):
        if checked:
            self._serialProxy.start()
        else:
            self._serialProxy.stop()

    def onButtonQuitClicked(self):
        if QMessageBox.warning(self, '警告', '你确定要退出软件吗？',
                               QMessageBox.Ok | QMessageBox.No) == QMessageBox.Ok:
            self.close()

    def onCurveDoubleClicked(self, checked):
        objectName = self.sender().objectName()
        if objectName == 'curveLBP':
            self.groupBoxTop.setVisible(checked)
            self.areaMiddle.setVisible(checked)
            self.curveLRP.setVisible(checked)
            self.curveRBP.setVisible(checked)
            self.curveRRP.setVisible(checked)
        elif objectName == 'curveLRP':
            self.groupBoxTop.setVisible(checked)
            self.areaMiddle.setVisible(checked)
            self.curveLBP.setVisible(checked)
            self.curveRBP.setVisible(checked)
            self.curveRRP.setVisible(checked)
        elif objectName == 'curveRBP':
            self.groupBoxTop.setVisible(checked)
            self.areaMiddle.setVisible(checked)
            self.curveLBP.setVisible(checked)
            self.curveLRP.setVisible(checked)
            self.curveRRP.setVisible(checked)
        elif objectName == 'curveRRP':
            self.groupBoxTop.setVisible(checked)
            self.areaMiddle.setVisible(checked)
            self.curveLBP.setVisible(checked)
            self.curveLRP.setVisible(checked)
            self.curveRBP.setVisible(checked)

    def onButtonLeftPowerStateChanged(self, checked):
        self._serialSend.ctrlWord.lPowerSw = checked
        self._serialSend.sum = SerialPortProxy.serialPortSendSum(self._serialSend)
        self._serialProxy.writeData(self._serialSend)

    def onButtonRightPowerStateChanged(self, checked):
        self._serialSend.ctrlWord.rPowerSw = checked
        self._serialSend.sum = SerialPortProxy.serialPortSendSum(self._serialSend)
        self._serialProxy.writeData(self._serialSend)

    def onButtonLeftTracksipStateChanged(self, checked):
        self._serialSend.ctrlWord.lTracksip = checked
        self._serialSend.sum = SerialPortProxy.serialPortSendSum(self._serialSend)
        self._serialProxy.writeData(self._serialSend)

    def onButtonRightTracksipStateChanged(self, checked):
        self._serialSend.ctrlWord.rTracksip = checked
        self._serialSend.sum = SerialPortProxy.serialPortSendSum(self._serialSend)
        self._serialProxy.writeData(self._serialSend)

    def onSerialStateChanged(self, info):
        text = '串口: [%s] %s' % (self._serialProxy.config().__str__(), info)
        print(text)

    def onSerialDisplayRespond(self, data, dateTime):
        # Major Brake Pressure
        suffix = self.editMLeftBrakeP.text().split(' ')[1]
        self.editMLeftBrakeP.setText('%.2f %s' % (data.lMBrakeP * 1.0, suffix))
        suffix = self.editMRightBrakeP.text().split(' ')[1]
        self.editMRightBrakeP.setText('%.2f %s' % (data.rMBrakeP * 1.0, suffix))
        # Minor Brake Pressure
        suffix = self.editALeftBrakeP.text().split(' ')[1]
        self.editALeftBrakeP.setText('%.2f %s' % (data.lABrakeP * 1.0, suffix))
        suffix = self.editARightBrakeP.text().split(' ')[1]
        self.editARightBrakeP.setText('%.2f %s' % (data.rABrakeP * 1.0, suffix))
        # Rotation Rate
        suffix = self.editLeftRotateRate.text().split(' ')[1]
        self.editLeftRotateRate.setText('%d %s' % (data.lWheelSpd * 1.0, suffix))
        suffix = self.editRightRotateRate.text().split(' ')[1]
        self.editRightRotateRate.setText('%d %s' % (data.rWheelSpd * 1.0, suffix))

        # curves
        timeT = dateTime.toMSecsSinceEpoch()

        # curve - LBP
        self.curveLBP.curve(0).sheft(QPoint(timeT, data.lMBrakeP))
        self.curveLBP.curve(1).sheft(QPoint(timeT, data.lABrakeP))
        # curve - LRP
        self.curveLRP.curve(0).sheft(QPoint(timeT, data.lWheelSpd))
        self.curveLRP.curve(1).sheft(QPoint(timeT, self._lTheorySpd))
        # curve - RBP
        self.curveRBP.curve(0).sheft(QPoint(timeT, data.rMBrakeP))
        self.curveRBP.curve(1).sheft(QPoint(timeT, data.rABrakeP))
        # curve - RRP
        self.curveRRP.curve(0).sheft(QPoint(timeT, data.rWheelSpd))
        self.curveRRP.curve(1).sheft(QPoint(timeT, self._rTheorySpd))

        if not DatabaseMgr().write(data, self._lTheorySpd, self._rTheorySpd, timeT):
            assert(False)

    def onSerialPortError(self, error, info):
        buttonPortState = self.findChild(JSwitchButton, 'buttonPortState')
        if not buttonPortState:
            return
        if error == QSerialPort.NoError:
            buttonPortState.setState(True)
        else:
            buttonPortState.setState(False)
        text = '串口: [%s] %s' % (self._serialProxy.config().__str__(), info)
        print(text)

    def execSliderWidget(self):
        sliderWidget = SliderWidget(self.sender().text(), self)
        objName = self.sender().objectName()
        if objName == 'buttonLeftSpeedGain':
            lineEdit = self.findChild(QLineEdit, 'editTheoryLeftRotateRate')
            text = lineEdit.text().split(' ')
            suffix = ' ' + text[1]
            curValue = float(text[0])
            sliderWidget.setRange(0, 3000)
            # sliderWidget.setDecimals(2)
            # sliderWidget.setSingleStep(0.01)
            sliderWidget.setSuffix(suffix)
            sliderWidget.setValue(curValue)

            def valueChanged(value):
                self._lTheorySpd = value
                buttonLeftSpeedSwitch = self.findChild(QPushButton, 'buttonLeftSpeedSwitch')
                if buttonLeftSpeedSwitch:
                    buttonLeftSpeedSwitch.setText('左转速关' if value == 0.0 else '左转速开');
                lineEdit.setText(('%.2f' % value) + suffix)

                # send
                self._serialSend.index += 1
                self._serialSend.lWheelSpd = int(value * 42.94967296)
                self._serialSend.sum = SerialPortProxy.serialPortSendSum(self._serialSend)
                self._serialProxy.writeData(self._serialSend)

            sliderWidget.valueChanged.connect(valueChanged)

        elif objName == 'buttonRightSpeedGain':
            lineEdit = self.findChild(QLineEdit, 'editTheoryRightRotateRate')
            text = lineEdit.text().split(' ')
            suffix = ' ' + text[1]
            curValue = float(text[0])
            sliderWidget.setRange(0, 3000)
            # sliderWidget.setDecimals(2)
            # sliderWidget.setSingleStep(0.01)
            sliderWidget.setSuffix(suffix)
            sliderWidget.setValue(curValue)

            def valueChanged(value):
                self._rTheorySpd = value
                buttonRightSpeedSwitch = self.findChild(QPushButton, 'buttonRightSpeedSwitch')
                if buttonRightSpeedSwitch:
                    buttonRightSpeedSwitch.setText('右转速关' if value == 0.0 else '右转速开')
                lineEdit.setText(('%.2f' % value) + suffix)

                # send
                self._serialSend.index += 1
                self._serialSend.rWheelSpd = int(value * 42.94967296)
                self._serialSend.sum = SerialPortProxy.serialPortSendSum(self._serialSend)
                self._serialProxy.writeData(self._serialSend)

            sliderWidget.valueChanged.connect(valueChanged)

        elif objName == 'buttonLeftSpeedKnob':
            lineEdit = self.findChild(QLineEdit, 'editTheoryLeftRotateRate')
            text = lineEdit.text().split(' ')
            suffix = ' ' + text[1]
            curValue = float(text[0])
            minValue = max(0, curValue - 50)
            maxValue = min(3000, curValue + 50)
            if maxValue < 100:
                maxValue = minValue + 100
            if minValue > 3000 - 100:
                minValue = maxValue - 100
            sliderWidget.setRange(minValue, maxValue)
            # sliderWidget.setDecimals(2)
            # sliderWidget.setSingleStep(0.01)
            sliderWidget.setSuffix(suffix)
            sliderWidget.setValue(curValue)

            def valueChanged(value):
                self._lTheorySpd = value
                buttonLeftSpeedSwitch = self.findChild(QPushButton, 'buttonLeftSpeedSwitch')
                if buttonLeftSpeedSwitch:
                    buttonLeftSpeedSwitch.setText('左转速关' if value == 0.0 else '左转速开')
                lineEdit.setText(('%.2f' % value) + suffix)

                # send
                self._serialSend.index += 1
                self._serialSend.lWheelSpd = int(value * 42.94967296)
                self._serialSend.sum = SerialPortProxy.serialPortSendSum(self._serialSend)
                self._serialProxy.writeData(self._serialSend)

            sliderWidget.valueChanged.connect(valueChanged)

        elif objName == 'buttonRightSpeedKnob':
            lineEdit = self.findChild(QLineEdit, 'editTheoryRightRotateRate')
            text = lineEdit.text().split(' ')
            suffix = ' ' + text[1]
            curValue = float(text[0])
            minValue = max(0, curValue - 50)
            maxValue = min(3000, curValue + 50)
            if maxValue < 100:
                maxValue = minValue + 100
            if minValue > 3000 - 100:
                minValue = maxValue - 100
            sliderWidget.setRange(minValue, maxValue)
            # sliderWidget.setDecimals(2)
            # sliderWidget.setSingleStep(0.01)
            sliderWidget.setSuffix(suffix)
            sliderWidget.setValue(curValue)

            def valueChanged(value):
                self._rTheorySpd = value
                buttonRightSpeedSwitch = self.findChild(QPushButton, 'buttonRightSpeedSwitch')
                if buttonRightSpeedSwitch:
                    buttonRightSpeedSwitch.setText('右转速关' if value == 0.0 else '右转速开');
                lineEdit.setText(('%.2f' % value) + suffix)

                # send
                self._serialSend.index += 1
                self._serialSend.rWheelSpd = int(value * 42.94967296)
                self._serialSend.sum = SerialPortProxy.serialPortSendSum(self._serialSend)
                self._serialProxy.writeData(self._serialSend)

            sliderWidget.valueChanged.connect(valueChanged)

        else:
            pass

        sliderWidget.exec_()

def disposeRC():
    res_image_rc.qCleanupResources()

if __name__ == '__main__':
    import sys
    from PyQt5.Qt import QApplication, QTranslator, QFile, QTextStream, QFont
    import resource.qss.res_qss_rc as res_qss_rc

    # modify
    sys.path[0] = applicationDirPath() + '/../../..'  # dir: root of carmonitor

    app = QApplication(sys.argv)
    app.setOrganizationDomain('com.smartsoft.iclosure')
    app.setOrganizationName('smartsoft')
    app.setApplicationName('carmonitor')
    app.setApplicationDisplayName('飞行器遥控️系统')
    app.setApplicationVersion('%d.%d.%d' % (1, 0, 0))

    # translator - system
    translator = QTranslator()
    translator.load(':/lang/qt_' + 'zh_CN')
    app.installTranslator(translator)

    # translator
    translator = QTranslator()
    translator.load(':/lang/' + 'zh_CN')
    app.installTranslator(translator)

    # style sheet
    file = QFile(":/qss/stylesheet-1.qss")
    if file.open(QFile.ReadOnly | QFile.Text):
        app.setStyleSheet(QTextStream(file).readAll())

    app.setFont(QFont('微软雅黑', 15))

    mainWidget = MainWidget()
    mainWidget.showMaximized()
    sys.exit(app.exec_())

    def disposeRC():
        res_qss_rc.qCleanupResources()
