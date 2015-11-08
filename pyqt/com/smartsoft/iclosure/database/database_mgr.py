'''
Created on Apr 25, 2015

@author: iclosure
'''
from com.smartsoft.utils.singleton import singleton
from PyQt5.QtSql import QSqlDatabase, QSqlQuery
from PyQt5.Qt import QApplication, QDir, QPointF, QFileInfo, QFile, QTextStream,\
    QDateTime, QProgressDialog
from com.smartsoft.utils.utils import applicationDirPath

@singleton
class DatabaseMgr:
    '''
    class DatabaseMgr
    '''
    
    def __init__(self):
        self._name = ''
        self._isOpen = False
        dataDir = applicationDirPath() + '/../data'
        if not QDir(dataDir).exists():
            QDir().mkdir(dataDir)
        # initialize SQL
        self._db = QSqlDatabase.addDatabase('QSQLITE')
        if not self._db.isValid():
            assert(False)
            
        # set account
        self._db.setUserName('611')
        self._db.setPassword('611')
    
    def create(self, name):
        if not name.strip():
            return False
        if not self._db.isValid():
            return False
        # has open?
        if self._db.isOpen() and self._db.databaseName() == name:
            return True
        self._name = name
        #
        self._db.setDatabaseName(name)
        if not self._db.open():
            assert(False)
            return False
        #create table
        return QSqlQuery(self._db).exec('create table tbl_recv_data('
                                        'msTime integer(20) primary key'
                                        ',lMBrakeP integer(5)'
                                        ',lABrakeP integer(5)'
                                        ',rMBrakeP integer(5)'
                                        ',rABrakeP integer(5)'
                                        ',lWheelSpd integer(6)'
                                        ',rWheelSpd integer(6)'
                                        ',lTheorySpd integer(6)'
                                        ',rTheorySpd integer(6)'
                                        ')')
    
    def open(self, name=''):
        if not self._db.isValid():
            return False
        # has open?
        if self._db.isOpen():
            if not name.strip():
                return True
            else:
                self._db.close()
        # set account
        if name.strip():
            self._db.setUserName('611')
            self._db.setPassword('611')
            self._db.setDatabaseName(name)
            self._name = name
        # open SQL
        if not self._db.open():
            assert(False)
            return False
        return True
    
    def close(self):
        if not self._db.isValid():
            return
        self._db.close()
    
    def save(self):
        self._isOpen = self._db.isOpen()
        self._name = self._db.databaseName()
        self._db.close()
    
    def restore(self):
        self.close()
        self._db.setDatabaseName(self._name)
        if self._isOpen:
            self._db.open()
    
    def startTime(self):
        if not (self._db.isValid() and self._db.isOpen()):
            return 0
        query = QSqlQuery(self._db)
        if not query.exec('select min(msTime) from tbl_recv_data'):
            return 0
        if not query.next():
            return 0
        return query.value(0)
    
    def endTime(self):
        if not (self._db.isValid() and self._db.isOpen()):
            return 0
        query = QSqlQuery(self._db)
        if not query.exec('select max(msTime) from tbl_recv_data'):
            return 0
        if not query.next():
            return 0
        return query.value(0)
    
    def read(self, section, points, startTime=0, endTime=0):
        if not (self._db.isValid() and self._db.isOpen()):
            return False
        query = QSqlQuery(self._db)
        if startTime == 0 and endTime == 0:
            strQuery = 'select msTime,%s from tbl_recv_data order by msTime' % section
        else:
            strQuery = 'select msTime,%s from tbl_recv_data '
            'where (msTime>=%d and msTime<=%d) order by msTime' % \
            (section, startTime, endTime)
        if not query.exec(strQuery):
            return False
        points.clear()
        while query.next():
            points.append(QPointF(query.value('msTime'), query.value(section)))
        return True
    
    def write(self, data, lTheorySpd, rTheorySpd, timeT, async=False):
        if not (self._db.isValid() and self._db.isOpen()):
            return False
        return QSqlQuery(self._db).exec('insert into tbl_recv_data values'
                                        '(%d,%d,%d,%d,%d,%d,%d,%d,%d)' %
                                         (timeT,
                                          data.lMBrakeP,
                                          data.lABrakeP,
                                          data.rMBrakeP,
                                          data.rABrakeP,
                                          data.lWheelSpd,
                                          data.rWheelSpd,
                                          lTheorySpd,
                                          rTheorySpd
                                          ))
    
    def convertToText(self, filePaths):
        if not filePaths:
            return True
        # progress dialog
        progressDialog = QProgressDialog('数据库文件格式转换中，请稍候……',
                                      '取消', 0, len(filePaths) - 1)
        progressDialog.setStyleSheet('QLabel{color:black;}')
        progressDialog.setCancelButton(None)
        progressDialog.setAutoClose(False)
        progressDialog.setAutoReset(False)
        progressDialog.show()
        # make director
        targetDir = applicationDirPath() + '/../data/conv/'
        if not QDir(targetDir).exists():
            QDir().mkdir(targetDir)
        import os
        if os.sys.platform == 'win32':
            newLine = '\r\n'
        else:
            newLine = '\r'
        for (i, filePath) in enumerate(filePaths):
            fileInfo = QFileInfo(filePath)
            progressDialog.setLabelText('正在转换文件 %s ……' % fileInfo.fileName())
            progressDialog.setValue(i)
            QApplication.processEvents()
            #
            if not (filePath.endswith('.db') and self.open(filePath)):
                continue
            targetFile = QFile(targetDir + fileInfo.baseName() + '.txt')
            if not targetFile.open(QFile.WriteOnly):
                self.close()
                continue
            query = QSqlQuery(self._db)
            if not query.exec('select * from tbl_recv_data order by msTime'):
                targetFile.close()
                self.close()
                continue
            textStream = QTextStream(targetFile)
            textStream << \
                ' * time' << \
                ' * lMBrakeP' << \
                ' * lABrakeP' << \
                ' * rMBrakeP' << \
                ' * rABrakeP' << \
                ' * lWheelSpd' << \
                ' * rWheelSpd' << \
                ' * lTheorySpd' << \
                ' * rTheorySpd' << \
                newLine
            while query.next():
                textStream \
                    << QDateTime.fromMSecsSinceEpoch(query.value(0)).toString('yyyy-MM-dd HH:mm:ss') << ' -> ' \
                    << query.value(1) << ' ' \
                    << query.value(2) << ' ' \
                    << query.value(3) << ' ' \
                    << query.value(4) << ' ' \
                    << query.value(5) << ' ' \
                    << query.value(6) << ' ' \
                    << query.value(7) << ' ' \
                    << query.value(8) << newLine
            self.close()
        return True
        
if __name__ == '__main__':
    one = DatabaseMgr()
    two = DatabaseMgr()
    print(id(one), id(two))
    