'''
Created on Apr 5, 2015

@author: iclosure
'''

from PyQt5.Qt import QApplication, QTranslator, QFile, QFont, QTextStream
from com.smartsoft.iclosure.main_widget import MainWidget
import resource.qss.res_qss_rc as res_qss_rc
import json

if __name__ == '__main__': 
    import sys
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
        
    app.setFont(QFont('Microsoft Yahei', 15))
    
    # main window
    mainWidget = MainWidget()
    mainWidget.showMaximized()
        
        #json string:
    s = json.loads('{"name":"test", "type":{"name":"seq", "parameter":["1", "2"]}}')
    print(s)
    print(s.keys())
    print(s["name"])
    print(s["type"]["name"])
    print(s["type"]["parameter"][1])
    
    sys.exit(app.exec_())
    
def disposeRC():
    res_qss_rc.qCleanupResources()
    