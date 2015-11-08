'''
Created on Apr 5, 2015

@author: iclosure
'''

from PyQt5 import QtCore
from PyQt5.QtCore import Qt, QRect
from PyQt5.Qt import QWidget, QIcon, qApp, QGroupBox, QPushButton, QHBoxLayout,\
    QPixmap, QPainter, QVBoxLayout, QLabel, QGridLayout, QLineEdit, QMessageBox
import resource.image.res_image_rc as res_image_rc
from com.smartsoft.iclosure.curve_widget import CurveWidget

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
        
    @QtCore.pyqtSlot(bool)
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
        
    @QtCore.pyqtSlot(bool)
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
        self.lTheorySpd = 0
        self.rTheorySpd = 0
        
        #self.onClicked1 = pyqtSignal(int)
        
        # mainWindow properties
        self.setObjectName('MainWidget')
        self.setWindowIcon(QIcon(':/image/default/app.icon'))
        self.setWindowTitle('%s V%s' % (
                            qApp.applicationDisplayName(),
                            qApp.applicationVersion()))
        self.resize(800, 480)
        
        # mainWindow layout
        
        # top
        
        groupBoxTop = QGroupBox(self)
        groupBoxTop.setObjectName('groupBoxTop')
        
        # command dashboard
        buttonLeftPower = JSwitchButton(parent=groupBoxTop)
        buttonRightPower = JSwitchButton(parent=groupBoxTop)
        buttonSettings = QPushButton(groupBoxTop)
        buttonHistory = QPushButton(groupBoxTop)
        buttonQuit = QPushButton(groupBoxTop)
        
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
        
        buttonPortState = JSwitchButton(pixmap=QPixmap(':/carmonitor/image/button-port-state.png'), parent=areaPortState)
        buttonPortState.setObjectName('buttonPortState')
        vertLayoutPortState.addWidget(QLabel('串口', areaPortState), 0, Qt.AlignHCenter)
        vertLayoutPortState.addWidget(buttonPortState)
        
        # 
        horiLayoutTop = QHBoxLayout(groupBoxTop)
        horiLayoutTop.setContentsMargins(0, 0, 0, 0)
        horiLayoutTop.addSpacing(25)
        horiLayoutTop.addWidget(buttonSettings, 0, Qt.AlignLeft)
        horiLayoutTop.addSpacing(20)
        horiLayoutTop.addWidget(buttonHistory, 0, Qt.AlignLeft)
        horiLayoutTop.addSpacing(65)
        horiLayoutTop.addWidget(buttonLeftPower)
        horiLayoutTop.addWidget(QLabel('左电源开关', groupBoxTop))
        horiLayoutTop.addStretch()
        horiLayoutTop.addWidget(areaPortState, 0, Qt.AlignTop)
        horiLayoutTop.addStretch()
        horiLayoutTop.addWidget(QLabel('右电源开关', groupBoxTop))
        horiLayoutTop.addWidget(buttonRightPower)
        horiLayoutTop.addSpacing(150)
        horiLayoutTop.addWidget(buttonQuit, 0, Qt.AlignRight)
        horiLayoutTop.addSpacing(25)
        
        # middle
        
        # curves
        curveLBP = CurveWidget('左刹车压力（MPa）', self)
        curveLRP = CurveWidget('左转速（r/min）', self)
        curveRBP = CurveWidget('右刹车压力（MPa）', self)
        curveRRP = CurveWidget('右转速（r/min）', self)
        
        #curveLBP.setAxisScale(QwtPlot.yLeft, 0, 30, 5) 

        # areaMiddle
        areaMiddle = QWidget(self)
        areaMiddle.setObjectName('areaMiddle')
        areaMiddle.setFixedWidth(280)
        
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
        editMLeftBrakeP = QLineEdit(groupBoxStatus)
        editMRightBrakeP = QLineEdit(groupBoxStatus)
        editMLeftBrakeP.setObjectName('editMLeftBrakeP')
        editMRightBrakeP.setObjectName('editMRightBrakeP')
        editMLeftBrakeP.setReadOnly(True)
        editMRightBrakeP.setReadOnly(True)
        gridLayoutStatus.addWidget(QLabel('主刹车压力：', groupBoxStatus),
                                   2, 0, 1, 2, Qt.AlignCenter)
        gridLayoutStatus.addWidget(editMLeftBrakeP, 3, 0, 1, 1)
        gridLayoutStatus.addWidget(editMRightBrakeP, 3, 1, 1, 1)
        
        # Assistant Brake Pressure
        editALeftBrakeP = QLineEdit(groupBoxStatus)
        editARightBrakeP = QLineEdit(groupBoxStatus)
        editALeftBrakeP.setObjectName('editALeftBrakeP')
        editARightBrakeP.setObjectName('editARightBrakeP')
        editALeftBrakeP.setReadOnly(True)
        editARightBrakeP.setReadOnly(True)
        gridLayoutStatus.addWidget(QLabel('副刹车压力：', groupBoxStatus),
                                   4, 0, 1, 2, Qt.AlignCenter)
        gridLayoutStatus.addWidget(editALeftBrakeP, 5, 0, 1, 1)
        gridLayoutStatus.addWidget(editARightBrakeP, 5, 1, 1, 1)
        
        # Rotation Rate
        editLeftRotateRate = QLineEdit(groupBoxStatus)
        editRightRotateRate = QLineEdit(groupBoxStatus)
        editLeftRotateRate.setObjectName('editLeftRotateRate')
        editRightRotateRate.setObjectName('editRightRotateRate')
        gridLayoutStatus.addWidget(QLabel('实际转速：', groupBoxStatus),
                                   6, 0, 1, 2, Qt.AlignCenter)
        gridLayoutStatus.addWidget(editLeftRotateRate, 7, 0, 1, 1)
        gridLayoutStatus.addWidget(editRightRotateRate, 7, 1, 1, 1)
        
        # Theory Rotation Rate
        editTheoryLeftRotateRate = QLineEdit(groupBoxStatus)
        editTheoryRightRotateRate = QLineEdit(groupBoxStatus)
        editTheoryLeftRotateRate.setObjectName('editTheoryLeftRotateRate')
        editTheoryRightRotateRate.setObjectName('editTheoryRightRotateRate')
        gridLayoutStatus.addWidget(QLabel('理论转速：', groupBoxStatus),
                                   8, 0, 1, 2, Qt.AlignCenter)
        gridLayoutStatus.addWidget(editTheoryLeftRotateRate, 9, 0, 1, 1)
        gridLayoutStatus.addWidget(editTheoryRightRotateRate, 9, 1, 1, 1)
        
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
        buttonLeftTracksip = JTracksipButton(parent=groupBoxCtrl)
        buttonLeftDashboard.setObjectName('buttonLeftDashboard')
        buttonLeftSpeedGain.setObjectName('buttonLeftSpeedGain')
        buttonLeftSpeedKnob.setObjectName('buttonLeftSpeedKnob')
        buttonLeftTracksip.setObjectName('buttonLeftTracksip')
        buttonLeftTracksip.setFixedSize(110, 45)
        
        # right-button
        buttonRightDashboard = JDashButton('右指令旋钮', groupBoxCtrl)
        buttonRightSpeedGain = JDashButton('右转速增益', groupBoxCtrl)
        buttonRightSpeedKnob = JDashButton('右转速增益', groupBoxCtrl)
        buttonRightTracksip = JTracksipButton(parent=groupBoxCtrl)
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
        vertLayoutMid = QVBoxLayout(areaMiddle)
        vertLayoutMid.setContentsMargins(0, 0, 0, 0)
        vertLayoutMid.setSpacing(0)
        vertLayoutMid.addWidget(groupBoxStatus)
        vertLayoutMid.addWidget(groupBoxCtrl)
        vertLayoutMid.addSpacing(20)
        
        # 
        gridLayoutBottom = QGridLayout()
        gridLayoutBottom.setContentsMargins(0, 0, 0, 0)
        gridLayoutBottom.setSpacing(1)
        gridLayoutBottom.addWidget(curveLBP, 0, 0, 1, 1)
        gridLayoutBottom.addWidget(curveLRP, 1, 0, 1, 1)
        gridLayoutBottom.addWidget(areaMiddle, 0, 1, 2, 1)
        gridLayoutBottom.addWidget(curveRBP, 0, 2, 1, 1)
        gridLayoutBottom.addWidget(curveRRP, 1, 2, 1, 1)
        
        # main-layout
        vertLayoutMain = QVBoxLayout(self)
        vertLayoutMain.setContentsMargins(5, 5, 5, 5)
        vertLayoutMain.addWidget(groupBoxTop)
        vertLayoutMain.addLayout(gridLayoutBottom)
        
        # global properties
        qApp.setProperty('MainWidget', self)
        
        #
        buttonSettings.clicked.connect(self.onButtonSettingsClicked)
        buttonHistory.clicked.connect(self.onButtonHistoryClicked)
        buttonPortState.clicked.connect(self.onButtonPortStateClicked)
        buttonQuit.clicked.connect(self.onButtonQuitClicked)
        
        # curves
        
        # switch-power
        buttonLeftPower.stateChanged.connect(self.onButtonLeftPowerStateChanged)
        buttonRightPower.stateChanged.connect(self.onButtonRightPowerStateChanged)
        
        # switch-tracksip
        buttonLeftTracksip.stateChanged.connect(self.onButtonLeftTracksipStateChanged)
        buttonRightTracksip.stateChanged.connect(self.onButtonRightTracksipStateChanged)
        
        #
        buttonLeftSpeedGain.clicked.connect(self.execSliderWidget)
        buttonLeftSpeedKnob.clicked.connect(self.execSliderWidget)
        buttonRightSpeedGain.clicked.connect(self.execSliderWidget)
        buttonRightSpeedKnob.clicked.connect(self.execSliderWidget)
        
        # final initialization
        
        editMLeftBrakeP.setText('0 MPa')
        editMRightBrakeP.setText('0 MPa')
        editALeftBrakeP.setText('0 MPa')
        editARightBrakeP.setText('0 MPa')
        
        editLeftRotateRate.setText('0 r/min')
        editRightRotateRate.setText('0 r/min')
        editTheoryLeftRotateRate.setText('0 r/min')
        editTheoryRightRotateRate.setText('0 r/min')
        
        #
    
    def onButtonSettingsClicked(self):
        print('button-settings-clicked')
    
    def onButtonHistoryClicked(self):
        print('button-history-clicked')
    
    def onButtonPortStateClicked(self, checked):
        print('button-port-state-clicked: %d' % checked)
    
    def onButtonQuitClicked(self):
        if QMessageBox.warning(self, '警告', '你确定要退出软件吗？', 
                               QMessageBox.Ok | QMessageBox.No) == QMessageBox.Ok:
            self.close()
            
    def onButtonLeftPowerStateChanged(self, checked):
        print('button-left-power-state-changed: %d' % checked)
        
    def onButtonRightPowerStateChanged(self, checked):
        print('button-right-power-state-changed: %d' % checked)
        
    def onButtonLeftTracksipStateChanged(self, checked):
        print('button-left-tracksip-state-changed: %d' % checked)
    
    def onButtonRightTracksipStateChanged(self, checked):
        print('button-right-tracksip-state-changed: %d' % checked)
    
    def execSliderWidget(self):
        pass

def disposeRC():
    res_image_rc.qCleanupResources()
        
if __name__ == '__main__':
    import sys
    from PyQt5.Qt import QApplication, QTranslator, QFile, QTextStream, QFont
    import resource.qss.res_qss_rc as res_qss_rc
    
    app = QApplication(sys.argv)
    app.setOrganizationDomain('com.smartsoft.iclosure')
    app.setOrganizationName('smartsoft')
    app.setApplicationName('carmonitor')
    app.setApplicationDisplayName('飞行器遥控️系统')
    app.setApplicationVersion('%d.%d.%d' % (1, 0, 0))
    
    # translator
    translator = QTranslator()
    translator.load(':/lang/' + 'zh_CN')
    app.installTranslator(translator)

    # style sheet
    file = QFile(":/qss/stylesheet-1")
    if file.open(QFile.ReadOnly | QFile.Text):
        app.setStyleSheet(QTextStream(file).readAll())
        
    app.setFont(QFont('微软雅黑', 12))
    
    mainWidget = MainWidget()
    mainWidget.showMaximized()
    sys.exit(app.exec_())
    
    def disposeRC():
        res_qss_rc.qCleanupResources()