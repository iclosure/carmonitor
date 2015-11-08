'''
Created on Apr 25, 2015

@author: iclosure
'''
from PyQt5.Qt import QWidget, QPointF

class Curve(QWidget):
    '''
    class Curve
    '''
    def __init__(self, title, parent = None):
        super(Curve, self).__init__(parent)
        self._sheftCount = 100
        self._points = QPointF() * 256

    def setCount(self, count):
        self._points = QPointF() * count

    def count(self):
        return self._points.count()

    def append(self, point):
        pass

    def setSamples(self, points):
        pass

    def sheft(self, point):
        pass

    def sheftY(self, y):
        pass

    def samples(self):
        return self._points

    def sheftCount(self):
        return self._sheftCount

    def setSheftCount(self, value):
        self._sheftCount = value

    def setLinePen(self, pen):
        pass

    def update(self):
        pass
