'''
Created on Apr 22, 2015

@author: iclosure
'''
from PyQt5.Qt import QWidget, qApp, QPen, QColor, QHBoxLayout, QPushButton, \
    QTableWidget
from PyQt5 import QtCore
# from PyQt5.Qwt6.Qwt import *
from com.smartsoft.iclosure.curve import Curve
'''
class TimeScaleDraw(QwtScaleDraw):
    def __init__(self):
        pass
    
    def setFormat(self, format):
        pass
    
    def label(self, value):
        pass
'''
class CurveWidget(QWidget):
    '''
    class CurveWidget
    '''
    doubleClicked = QtCore.pyqtSignal(bool)
    def __init__(self, title = None, zoom = False, parent = None):
        super(CurveWidget, self).__init__(parent)
        self.setObjectName('CurveWidget')
        self.setFixedSize(300, 300)
        self.setStyleSheet('QWidget#CurveWidget{background-color:rgba(100,100,100,200);}')
        self._enableDBClick = False
        self._showNormal = True
        self._curves = []

        #
        self.initialize(zoom)

        # TODO
        horiLayoutMain = QHBoxLayout(self)

        tableWidget = QTableWidget(self);
        horiLayoutMain.addWidget(tableWidget)

    def initialize(self, zoom):
        self.setContentsMargins(2, 2, 2, 2)
        self.setMinimumHeight(338)

        _width = qApp.desktop().availableGeometry().width()
        if _width == 1280:
            self._width = 493
        elif _width == 1366:
            self._width = 532
        else:
            self._width = 493

        self.setFixedWidth(self._width)

        # grid

        # marker

        # picker

        # zoomer
        if zoom:
            pass

        # scale

        # curves

        self.addCurve('Curve 1', QPen(QColor('#0c78a6'), 1))
        self.addCurve('Curve 2', QPen(QColor('#5fd43b'), 1))

        # legend

    def addCurve(self, title, pen, samples = None):
        curve = Curve(title, self)
        curve.setLinePen(pen)
        # curve.attach(self)
        curve.setSamples(samples)
        curve.setSheftCount(100)
        self._curves.append(curve)

    def removeCurve(self, curve):
        for (i, value) in enumerate(self._curves):
            if value == curve:
                # value.detach()
                del value
                self._curves.remove(value)
                break

    def autoScale(self):
        pass

    def clear(self):
        for curve in self._curves:
            # curve.detach()
            del curve
        self._curves.clear()

    def setTitle(self, text):
        pass

    def setCurveTitle(self, index, text):
        if index < 0 or index >= self._curves.__len__():
            return False
        self._curves[index].setTitle(text)
        return True

    def curve(self, index):
        if index < 0 or index >= self._curves.__len__():
            return None
        return self._curves[index]

    def curves(self):
        return self._curves

    def setScaleLabelFormat(self, format):
        pass

    def enableDBClick(self):
        return self._enableDBClick

    def setEnableDBClick(self, enable):
        self._enableDBClick = enable

    def mouseDoubleClickEvent(self, event):
        pass
