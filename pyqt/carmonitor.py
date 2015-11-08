'''
Created on Apr 5, 2015

@author: iclosure
'''

from PyQt5.Qt import QApplication, QTranslator, QFile, QFont, QTextStream
from com.smartsoft.iclosure.main_widget import MainWidget
import resource.qss.res_qss_rc as res_qss_rc

if __name__ == '__main__':
    import sys
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

    app.setFont(QFont('Microsoft Yahei', 12))

    # main window
    mainWidget = MainWidget()
    mainWidget.showMaximized()

    sys.exit(app.exec_())

def disposeRC():
    res_qss_rc.qCleanupResources()
