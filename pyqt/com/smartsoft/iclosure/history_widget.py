'''
Created on Apr 25, 2015

@author: iclosure
'''
from PyQt5.Qt import Qt, QDialog, QHBoxLayout, QPushButton, QFormLayout,\
    QDateTimeEdit, QCheckBox, QVBoxLayout, QFileDialog, QApplication, QDateTime,\
    QFileInfo, QMessageBox, QPen
from com.smartsoft.iclosure.curve_widget import CurveWidget
from com.smartsoft.iclosure.database.database_mgr import DatabaseMgr

class HistoryWidget(QDialog):
    '''
    class HistonWidget
    '''
    _v_curve_colors = [Qt.white,
                       Qt.gray,
                       Qt.red,
                       Qt.green,
                       Qt.blue,
                       Qt.cyan,
                       Qt.magenta,
                       Qt.darkYellow
                       ]
    _v_curve_checked = [True, True, True, True,
                        True, True, True, True]
    def __init__(self, parent=None):
        super(HistoryWidget, self).__init__(parent)
        self.setObjectName('HistoryWidget')
        self.resize(800, 480)
        self.setWindowTitle('历史数据查看')
        
        # layout - top
        horiLayoutTop = QHBoxLayout()
        
        buttonQuit = QPushButton(self)
        buttonQuit.setObjectName('buttonQuit')
        horiLayoutTop.addSpacing(25)
        horiLayoutTop.addWidget(buttonQuit, 0, Qt.AlignLeft)
        horiLayoutTop.addStretch()
        
        # button-export
        buttonExport = QPushButton(self)
        buttonExport.setObjectName('buttonExport')
        horiLayoutTop.addWidget(buttonExport)
        horiLayoutTop.addStretch()
        
        # button-open
        buttonOpen = QPushButton(self)
        buttonOpen.setObjectName('buttonOpen')
        horiLayoutTop.addWidget(buttonOpen)
        horiLayoutTop.addStretch()
        
        formLayoutTime = QFormLayout()
        formLayoutTime.setFormAlignment(Qt.AlignVCenter)
        horiLayoutTop.addLayout(formLayoutTime)
        horiLayoutTop.addStretch()
        
        self.dateTimeEditStart = QDateTimeEdit(self)
        self.dateTimeEditStart.setObjectName('dateTimeEditStart')
        self.dateTimeEditStart.setDisplayFormat('yyyy-MM-dd HH:mm:ss')
        formLayoutTime.addRow('起始时间：', self.dateTimeEditStart)
        
        self.dateTimeEditEnd = QDateTimeEdit(self)
        self.dateTimeEditEnd.setObjectName('dateTimeEditEnd')
        self.dateTimeEditEnd.setDisplayFormat('yyyy-MM-dd HH:mm:ss')
        formLayoutTime.addRow('结束时间：', self.dateTimeEditEnd)
        
        # LBP
        formLayoutLBP = QFormLayout()
        formLayoutLBP.setFormAlignment(Qt.AlignVCenter)
        formLayoutLBP.setLabelAlignment(Qt.AlignRight)
        horiLayoutTop.addLayout(formLayoutLBP)
        horiLayoutTop.addStretch()
        self.checkBoxLBPMajor = QCheckBox('主', self)
        self.checkBoxLBPMajor.setProperty('curveColor', '#101010')
        self.checkBoxLBPMinor = QCheckBox('副', self)
        self.checkBoxLBPMinor.setProperty('curveColor', '#101010')
        formLayoutLBP.addRow('左刹车压力:', self.checkBoxLBPMajor)
        formLayoutLBP.addRow('', self.checkBoxLBPMinor)
        
        # RBP
        formLayoutRBP = QFormLayout()
        formLayoutRBP.setFormAlignment(Qt.AlignVCenter)
        formLayoutRBP.setLabelAlignment(Qt.AlignRight)
        horiLayoutTop.addLayout(formLayoutRBP)
        horiLayoutTop.addStretch()
        self.checkBoxRBPMajor = QCheckBox('主', self)
        self.checkBoxRBPMajor.setProperty('curveColor', '#101010')
        self.checkBoxRBPMinor = QCheckBox('副', self)
        self.checkBoxRBPMinor.setProperty('curveColor', '#101010')
        formLayoutRBP.addRow('右刹车压力:', self.checkBoxRBPMajor)
        formLayoutRBP.addRow('', self.checkBoxRBPMinor)
        
        # LRP
        formLayoutLRP = QFormLayout()
        formLayoutLRP.setFormAlignment(Qt.AlignVCenter)
        formLayoutLRP.setLabelAlignment(Qt.AlignRight)
        horiLayoutTop.addLayout(formLayoutLRP)
        horiLayoutTop.addStretch()
        self.checkBoxLRPTheory = QCheckBox('理论', self)
        self.checkBoxLRPTheory.setProperty('curveColor', '#101010')
        self.checkBoxLRPReal = QCheckBox('实际', self)
        self.checkBoxLRPReal.setProperty('curveColor', '#101010')
        formLayoutLRP.addRow('左转速:', self.checkBoxLRPTheory)
        formLayoutLRP.addRow('', self.checkBoxLRPReal)
        
        # RRP
        formLayoutRRP = QFormLayout()
        formLayoutRRP.setFormAlignment(Qt.AlignVCenter)
        formLayoutRRP.setLabelAlignment(Qt.AlignRight)
        horiLayoutTop.addLayout(formLayoutRRP)
        horiLayoutTop.addStretch()
        self.checkBoxRRPTheory = QCheckBox('理论', self)
        self.checkBoxRRPTheory.setProperty('curveColor', '#101010')
        self.checkBoxRRPReal = QCheckBox('实际', self)
        self.checkBoxRRPReal.setProperty('curveColor', '#101010')
        formLayoutRRP.addRow('右转速:', self.checkBoxRRPTheory)
        formLayoutRRP.addRow('', self.checkBoxRRPReal)
        
        # button-update
        buttonUpdate = QPushButton(self)
        buttonUpdate.setObjectName('buttonUpdate')
        horiLayoutTop.addWidget(buttonUpdate)
        horiLayoutTop.addStretch()
        
        # middle-curves
        self.curveHistory = CurveWidget('历史数据回放', True, self)
        self.curveHistory.setMaximumWidth(10e5)
        self.curveHistory.setScaleLabelFormat('yyyy/MM/dd\n  HH:mm:ss')
        self.curveHistory.clear()
        
        #
        vertLayoutMain = QVBoxLayout(self)
        vertLayoutMain.addLayout(horiLayoutTop)
        vertLayoutMain.addWidget(self.curveHistory)
        
        buttonQuit.clicked.connect(self.accept)
        buttonOpen.clicked.connect(self.buttonOpenClicked)
        buttonExport.clicked.connect(self.buttonExportClicked)
        self.dateTimeEditStart.dateTimeChanged.connect(self.dateTimeStartChanged)
        self.dateTimeEditEnd.dateTimeChanged.connect(self.dateTimeEndChanged)
        self.checkBoxLBPMajor.toggled.connect(self.checkBoxLBPMajorToggled)
        self.checkBoxLBPMinor.toggled.connect(self.checkBoxLBPMinorToggled)
        self.checkBoxRBPMajor.toggled.connect(self.checkBoxRBPMajorToggled)
        self.checkBoxRBPMinor.toggled.connect(self.checkBoxRBPMinorToggled)
        self.checkBoxLRPTheory.toggled.connect(self.checkBoxLRPTheoryToggled)
        self.checkBoxLRPReal.toggled.connect(self.checkBoxLRPRealToggled)
        self.checkBoxRRPTheory.toggled.connect(self.checkBoxRRPTheoryToggled)
        self.checkBoxRRPReal.toggled.connect(self.checkBoxRRPRealToggled)
        buttonUpdate.clicked.connect(self.buttonUpdateClicked)
        
        # finalLy initialize
        self.checkBoxLBPMajor.setChecked(self._v_curve_checked[0]);
        self.checkBoxLBPMinor.setChecked(self._v_curve_checked[1]);
        self.checkBoxRBPMajor.setChecked(self._v_curve_checked[2]);
        self.checkBoxRBPMinor.setChecked(self._v_curve_checked[3]);
        self.checkBoxLRPTheory.setChecked(self._v_curve_checked[4]);
        self.checkBoxLRPReal.setChecked(self._v_curve_checked[5]);
        self.checkBoxRRPTheory.setChecked(self._v_curve_checked[6]);
        self.checkBoxRRPReal.setChecked(self._v_curve_checked[7]);
        
    def buttonOpenClicked(self):
        fileDialog = QFileDialog(self,
                                 '打开历史数据文件',
                                 QApplication.applicationDirPath() + '/../data',
                                 'Database File (*.db *.mdb)')
        if fileDialog.exec_() == QDialog.Rejected:
            return
        # clear curve
        self.curveHistory.clear()
        # 
        filePaths = fileDialog.selectedFiles()
        if filePaths.isEmpty():
            return
        filePath = filePaths.first()
        if filePath.isEmpty():
            return
        # open database
        if not DatabaseMgr().open(filePath):
            return
        #
        startTime = QDateTime.fromMSecsSinceEpoch(DatabaseMgr().startTime())
        endTime = QDateTime.fromMSecsSinceEpoch(DatabaseMgr().endTime())
        self.dateTimeEditStart.setDateTimeRange(startTime, endTime)
        self.dateTimeEditEnd.setDateTimeRange(startTime, endTime)
        self.dateTimeEditEnd.setDateTime(endTime)
        self.dateTimeEditStart.setDateTime(startTime)
        
        # title of curve
        self.curveHistory.setTitle('历史数据回放' + '（' + QFileInfo(filePath).fileName() + '）')
        
    def buttonExportClicked(self):
        (filePaths, filter) = QFileDialog.getOpenFileNames(parent=self, 
                                                caption='转换数据库文件为文本格式',
                                                directory=QApplication.applicationDirPath() + '/../data',
                                                filter='Database file (*.db * mdb)')
        if not filePaths:
            return
        # 
        if DatabaseMgr().convertToText(filePaths):
            QMessageBox.information(self, '格式转换', '转换成功！')
        else:
            QMessageBox.warning(self, '格式转换', '转换失败！')
    
    def dateTimeStartChanged(self, dateTime):
        dateTimeEnd = self.dateTimeEditEnd.dateTime()
        if dateTime > dateTimeEnd:
            self.dateTimeEditStart.setDateTime(dateTimeEnd)
    
    def dateTimeEndChanged(self, dateTime):
        dateTimeStart = self.dateTimeEditStart.dateTime()
        if dateTime < dateTimeStart:
            self.dateTimeEditEnd.setDateTime(dateTimeStart)
    
    def checkBoxLBPMajorToggled(self, checked):
        self._v_curve_checked[0] = checked
    
    def checkBoxLBPMinorToggled(self, checked):
        self._v_curve_checked[1] = checked
    
    def checkBoxRBPMajorToggled(self, checked):
        self._v_curve_checked[2] = checked
    
    def checkBoxRBPMinorToggled(self, checked):
        self._v_curve_checked[3] = checked
    
    def checkBoxLRPTheoryToggled(self, checked):
        self._v_curve_checked[4] = checked
    
    def checkBoxLRPRealToggled(self, checked):
        self._v_curve_checked[5] = checked
    
    def checkBoxRRPTheoryToggled(self, checked):
        self._v_curve_checked[6] = checked
    
    def checkBoxRRPRealToggled(self, checked):
        self._v_curve_checked[7] = checked
    
    def buttonUpdateClicked(self):
        # clear curve
        self.curveHistory.clear()
        
        points = []
        startTime = self.dateTimeEditStart.dateTime().toMSecsSinceEpoch()
        endTime = self.dateTimeEditEnd.dateTime().toMSecsSinceEpoch()
        
        # LBP-Major
        if self.checkBoxLBPMajor.isChecked():
            # read
            points.clear()
            if DatabaseMgr().read('lMBrakeP', points, startTime, endTime):
                self.curveHistory.addCurve('左刹压力－主', QPen(self.randomColor(0)), points)
        # LBP-Minor
        if self.checkBoxLBPMinor.isChecked():
            # read
            points.clear()
            if DatabaseMgr().read('lABrakeP', points, startTime, endTime):
                self.curveHistory.addCurve('左刹压力－副', QPen(self.randomColor(1)), points)
        # RBP-Major
        if self.checkBoxRBPMajor.isChecked():
            # read
            points.clear()
            if DatabaseMgr().read('rMBrakeP', points, startTime, endTime):
                self.curveHistory.addCurve('右刹压力－主', QPen(self.randomColor(2)), points)
        # RBP-Minor
        if self.checkBoxRBPMinor.isChecked():
            # read
            points.clear()
            if DatabaseMgr().read('rABrakeP', points, startTime, endTime):
                self.curveHistory.addCurve('右刹压力－副', QPen(self.randomColor(3)), points)
           
        # LRP-Theory
        if self.checkBoxLRPTheory.isChecked():
            # read
            points.clear()
            if DatabaseMgr().read('lTheorySpd', points, startTime, endTime):
                self.curveHistory.addCurve('左转速－理论', QPen(self.randomColor(4)), points)
        # LRP-Real
        if self.checkBoxLRPReal.isChecked():
            # read
            points.clear()
            if DatabaseMgr().read('lWheelSpd', points, startTime, endTime):
                self.curveHistory.addCurve('左转速－实际', QPen(self.randomColor(5)), points)
        # RRP-Theory
        if self.checkBoxRRPTheory.isChecked():
            # read
            points.clear()
            if DatabaseMgr().read('rTheorySpd', points, startTime, endTime):
                self.curveHistory.addCurve('右转速－理论', QPen(self.randomColor(6)), points)
        # RRP-Real
        if self.checkBoxRRPReal.isChecked():
            # read
            points.clear()
            if DatabaseMgr().read('rWheelSpd', points, startTime, endTime):
                self.curveHistory.addCurve('右转速－实际', QPen(self.randomColor(7)), points)
        
    @staticmethod
    def randomColor(index):
        return HistoryWidget._v_curve_colors[index]
    