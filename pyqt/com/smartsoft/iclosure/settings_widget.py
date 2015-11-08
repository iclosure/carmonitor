'''
Created on Apr 25, 2015

@author: iclosure
'''
from PyQt5.QtWidgets import QDialog
from PyQt5.Qt import Qt, QLabel, QPushButton, QHBoxLayout, \
    QVBoxLayout, QPoint, QComboBox, QGroupBox, QFormLayout
from PyQt5 import QtCore
from com.smartsoft.utils import styles

class JCloseButton(QPushButton):
    '''
    class JCloseButton
    '''
    def __init__(self, parent=None):
        super(JCloseButton, self).__init__(parent)

class SettingsWidget(QDialog):
    '''
    class SettingsWidget
    '''
    def __init__(self, parent=None):
        super(SettingsWidget, self).__init__(parent, Qt.FramelessWindowHint)
        self._mousePressed = False
        self._orgPos = QPoint(0, 0)
        self.setObjectName('SettingsWidget')
        
        #
        self.stylize()
        
        # main layout
        labelTitle = QLabel('设置', self)
        buttonClose = JCloseButton(self)
        buttonClose.setObjectName('buttonClose')
        buttonClose.setToolTip('关闭')
        
        horiLayoutTitle = QHBoxLayout()
        horiLayoutTitle.setContentsMargins(6, 0, 6, 6)
        horiLayoutTitle.addWidget(labelTitle, 0, Qt.AlignTop)
        horiLayoutTitle.addStretch()
        horiLayoutTitle.addWidget(buttonClose, 0, Qt.AlignTop)
        
        groupBoxSettings = QGroupBox('设置端口', self)
        groupBoxSettings.setObjectName('groupBoxSettings')
        formLayoutSettings = QFormLayout(groupBoxSettings)
        formLayoutSettings.setContentsMargins(40, 10, 40, 10)
        formLayoutSettings.setVerticalSpacing(20)
        formLayoutSettings.setLabelAlignment(Qt.AlignRight)
        
        self.comboBoxPort = QComboBox(self)
        self.comboBoxPort.setMinimumWidth(100)
        formLayoutSettings.addRow('端口号：', self.comboBoxPort)
        
        self.comboBoxBaudRate = QComboBox(self)
        self.comboBoxBaudRate.setMinimumWidth(100)
        formLayoutSettings.addRow('波特率：', self.comboBoxBaudRate)
        
        self.labelDataBits = QComboBox(self)
        self.labelDataBits.setMinimumWidth(100)
        formLayoutSettings.addRow('数据位：', self.labelDataBits)
        
        self.comboBoxParity = QComboBox(self)
        self.comboBoxParity.setMinimumWidth(100)
        formLayoutSettings.addRow('校验位：', self.comboBoxParity)
        
        self.comboBoxStopBits = QComboBox(self)
        self.comboBoxStopBits.setMinimumWidth(100)
        formLayoutSettings.addRow('停止位：', self.comboBoxStopBits)
        
        # all
        horiLayoutSettings = QHBoxLayout();
        horiLayoutSettings.addStretch();
        horiLayoutSettings.addWidget(groupBoxSettings);
        horiLayoutSettings.addStretch();
    
        buttonOk = QPushButton('确定', self)
        buttonOk.setObjectName('buttonOk')
        horiLayoutButtons = QHBoxLayout()
        horiLayoutButtons.addStretch()
        horiLayoutButtons.addWidget(buttonOk)
        
        vertLayoutMain = QVBoxLayout(self)
        vertLayoutMain.addLayout(horiLayoutTitle)
        vertLayoutMain.addSpacing(5)
        #vertLayoutMain.addWidget(groupBoxSettings)
        vertLayoutMain.addLayout(horiLayoutSettings)
        vertLayoutMain.addStretch()
        vertLayoutMain.addLayout(horiLayoutButtons)
        
        buttonClose.clicked.connect(self.close)
        
    @QtCore.pyqtSlot()
    def stylize(self):
        styles.stylize(self)
    
    def mousePressEvent(self, event):
        if event.button() == Qt.LeftButton:
            self._mousePressed = True
            self._orgPos = event.pos()
            self.setCursor(Qt.ClosedHandCursor)
        
    def mouseMoveEvent(self, event):
        if self._mousePressed:
            curPos = event.pos()
            curGeom= self.geometry()
            self.move(curGeom.topLeft() + curPos - self._orgPos)  
    
    def mouseReleaseEvent(self, event):
        self._mousePressed = False
        self.setCursor(Qt.ArrowCursor)
        