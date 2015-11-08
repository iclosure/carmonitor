'''
Created on Apr 25, 2015

@author: iclosure
'''

from PyQt5.QtCore import QSysInfo

def stylize(obj):
    import sys
    def setDefaultStyleSheet(obj):
        obj.setStyleSheet('QWidget#%s{'
                          ' border:1px solid rgb(100, 100, 64);'
                          ' border-radius:6px;'
                          ' background-color:rgb(32,32,32);'
                          '}' % obj.objectName())
    if sys.platform == 'darwin':
        if QSysInfo.macVersion() == QSysInfo.MV_10_9:
            pass
        else:
            setDefaultStyleSheet(obj)
    elif sys.platform == 'win32':
        if QSysInfo.windowsVersion() == QSysInfo.WV_WINDOWS7:
            #obj.setWindowFlags(Qt.CustomizeWindowHint)
            '''
            from PyQt5.Qt import QtWin, Qt
            if QtWin.isCompositionEnabled():
                QtWin.extendFrameIntoClientArea(obj, -1, -1, -1, -1)
                obj.setAttribute(Qt.WA_TranslucentBackground, True)
                obj.setAttribute(Qt.WA_NoSystemBackground, True)
                obj.setStyleSheet('%s{background:transparent;}' % obj.objectName())
            else:
                obj.setAttribute(Qt.WA_TranslucentBackground, True)
                obj.setAttribute(Qt.WA_NoSystemBackground, True)
                setDefaultStyleSheet(obj)
            '''
            setDefaultStyleSheet(obj)
        else:
            setDefaultStyleSheet(obj)
    else:
        setDefaultStyleSheet(obj)
        