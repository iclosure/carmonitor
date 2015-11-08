'''
Created on Apr 22, 2015

@author: iclosure
'''
from PyQt5.Qt import QWidget, qApp
from PyQt5.QtWidgets import QDesktopWidget

class CurveWidget(QWidget):
    '''
    class CurveWidget
    '''
    def __init__(self, title = None, parent = None):
        super(CurveWidget, self).__init__(parent)
        self.setObjectName('CurveWidget')
        #self.setFixedSize(QSize)
        
    def initialize(self):
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
        