'''
Created on Apr 25, 2015

@author: iclosure
'''

from PyQt5.QtWidgets import QDialog
from PyQt5.Qt import Qt, QLabel, QPushButton, QHBoxLayout, QDoubleSpinBox, \
    QSlider, QVBoxLayout, QPoint
from PyQt5 import QtCore
from com.smartsoft.utils import styles

class JCloseButton(QPushButton):
    '''
    class JCloseButton
    '''
    def __init__(self, parent = None):
        super(JCloseButton, self).__init__(parent)

class SliderWidget(QDialog):
    '''
    class SliderWidget
    '''
    valueChanged = QtCore.pyqtSignal(float)
    def __init__(self, title, parent = None):
        super(SliderWidget, self).__init__(parent, Qt.FramelessWindowHint)
        self._mousePressed = False
        self._orgPos = QPoint(0, 0)
        self.setObjectName('SliderWidget')
        self.resize(500, 150)

        #
        self.stylize()

        # main layout
        labelTitle = QLabel(title, self)
        buttonClose = JCloseButton(self)
        buttonClose.setObjectName('buttonClose')
        buttonClose.setToolTip('关闭')

        horiLayoutTitle = QHBoxLayout()
        horiLayoutTitle.setContentsMargins(6, 0, 6, 6)
        horiLayoutTitle.addWidget(labelTitle, 0, Qt.AlignTop)
        horiLayoutTitle.addStretch()
        horiLayoutTitle.addWidget(buttonClose, 0, Qt.AlignTop)

        self.doubleSpinBox = QDoubleSpinBox(self)
        self.doubleSpinBox.setObjectName('doubleSpinBox')
        self.doubleSpinBox.setMinimumWidth(200)
        self.doubleSpinBox.setRange(0, 6000)
        self.doubleSpinBox.setDecimals(2)
        self.doubleSpinBox.setSingleStep(0.01)

        self.slider = QSlider(Qt.Horizontal, self)
        self.slider.setObjectName('slider')
        self.slider.setRange(self.doubleSpinBox.minimum(),
                             self.doubleSpinBox.maximum())
        vertLayoutMain = QVBoxLayout(self)
        vertLayoutMain.addLayout(horiLayoutTitle)
        vertLayoutMain.addWidget(self.doubleSpinBox, 0, Qt.AlignHCenter)
        vertLayoutMain.addSpacing(5)
        vertLayoutMain.addWidget(self.slider)

        self.slider.rangeChanged.connect(self.doubleSpinBox.setRange)
        self.doubleSpinBox.valueChanged.connect(self.doubleSpinBoxValueChanged)
        self.slider.valueChanged.connect(self.setValue)
        buttonClose.clicked.connect(self.close)

    def setRange(self, minValue, maxValue):
        self.slider.setRange(minValue, maxValue)

    def setDecimals(self, prec):
        self.doubleSpinBox.setDecimals(prec)

    def setSingleStep(self, value):
        self.doubleSpinBox.setSingleStep(value)

    def setPrefix(self, prefix):
        self.doubleSpinBox.setPrefix(prefix)

    def setSuffix(self, suffix):
        self.doubleSpinBox.setSuffix(suffix)

    def doubleSpinBoxValueChanged(self, value):
        self.slider.setValue(value)
        self.valueChanged.emit(value)

    @QtCore.pyqtSlot(float)
    def setValue(self, value):
        self.doubleSpinBox.setValue(value)

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
            curGeom = self.geometry()
            self.move(curGeom.topLeft() + curPos - self._orgPos)

    def mouseReleaseEvent(self, event):
        self._mousePressed = False
        self.setCursor(Qt.ArrowCursor)
