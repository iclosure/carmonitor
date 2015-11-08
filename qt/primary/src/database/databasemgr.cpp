#include "precomp.h"
#include "databasemgr.h"

DataBaseMgr::DataBaseMgr()
{
    QString dataDir = QApplication::applicationDirPath().append("/../data");
    if (!QDir(dataDir).exists()) {
        QDir().mkdir(dataDir);
    }

    // initialize sql
    q_db = QSqlDatabase::addDatabase("QSQLITE");
    if (!q_db.isValid()) {
        Q_ASSERT(false);
    }

    // set account
    q_db.setUserName("611");
    q_db.setPassword("611");
}

DataBaseMgr::~DataBaseMgr()
{
    close();
}

DataBaseMgr &DataBaseMgr::instance()
{
    static DataBaseMgr _inst;
    return _inst;
}

bool DataBaseMgr::create(const QString &name)
{
    if (!q_db.isValid()) {
        return false;
    }

    // has open?
    if (q_db.isOpen() && q_db.databaseName() == name) {
        return true;
    }

    q_db.setDatabaseName(name);
    if (!q_db.open()) {
        Q_ASSERT(false);
        return false;
    }

    // create table
    return QSqlQuery(q_db).exec("create table tbl_recv_data("
                                "msTime integer(20) primary key"
                                ",lMBrakeP integer(5)"
                                ",lABrakeP integer(5)"
                                ",rMBrakeP integer(5)"
                                ",rABrakeP integer(5)"
                                ",lWheelSpd integer(6)"
                                ",rWheelSpd integer(6)"
                                ",lTheorySpd integer(6)"
                                ",rTheorySpd integer(6)"
                                ")");
}

bool DataBaseMgr::open(const QString &name)
{
    if (!q_db.isValid()) {
        return false;
    }

    // has open?
    if (q_db.isOpen()) {
        if (name.isEmpty()) {
            return true;
        } else {
            q_db.close();
        }
    }

    if (!name.isEmpty()) {
        // set account
        q_db.setUserName("611");
        q_db.setPassword("611");
        q_db.setDatabaseName(name);
    }

    if (!q_db.open()) {
        Q_ASSERT(false);
        return false;
    }

    return true;
}

void DataBaseMgr::close()
{
    if (!q_db.isValid()) {
        return;
    }

    q_db.close();
}

void DataBaseMgr::save()
{
    q_isOpen = q_db.isOpen();
    q_name = q_db.databaseName();
    this->close();
}

void DataBaseMgr::restore()
{
    this->close();
    q_db.setDatabaseName(q_name);
    if (q_isOpen) {
        q_db.open();
    }
}

quint64 DataBaseMgr::startTime() const
{
    if (!q_db.isValid()) {
        return 0;
    }

    QSqlQuery query(q_db);
    bool result = query.exec("select min(msTime) from tbl_recv_data");
    if (!result) {
        return 0;
    }

    if (!query.next()) {
        return 0;
    }

    return query.value(0).toULongLong();
}

quint64 DataBaseMgr::endTime() const
{
    if (!q_db.isValid()) {
        return 0;
    }

    QSqlQuery query(q_db);
    bool result = query.exec("select max(msTime) from tbl_recv_data");
    if (!result) {
        return 0;
    }

    if (!query.next()) {
        return 0;
    }

    return query.value(0).toULongLong();
}

bool DataBaseMgr::read(const QString &section, QVector<QPointF> &points,
                       const quint64 &startTime, const quint64 &endTime)
{
    QSqlQuery query(q_db);
    QString strQuery;

    if (startTime == 0 && endTime == 0) {
        strQuery = QString("select msTime,%1 from tbl_recv_data order by msTime").arg(section);
    } else {
        strQuery = QString("select msTime,%1 from tbl_recv_data "
                           "where (msTime >= %2 and msTime <= %3) "
                           "order by msTime")
                .arg(section)       // 1
                .arg(startTime)     // 2
                .arg(endTime);      // 3
    }

    bool result = query.exec(strQuery);
    if (!result) {
        return false;
    }

    //
    points.clear();
    while (query.next()) {
        points.append(QPointF(query.value("msTime").toULongLong(), query.value(section).toInt()));
    }

    return true;
}

bool DataBaseMgr::write(const SerialPortRecv &data, quint32 lTheorySpd, quint32 rTheorySpd,
                        const quint64 &time, bool async)
{
    Q_UNUSED(async);
    return QSqlQuery(q_db).exec(QString("insert into tbl_recv_data values(%1,%2,%3,%4,%5,%6,%7,%8,%9)")
                                .arg(time)
                                .arg(data.lMBrakeP)
                                .arg(data.lABrakeP)
                                .arg(data.rMBrakeP)
                                .arg(data.rABrakeP)
                                .arg(data.lWheelSpd)
                                .arg(data.rWheelSpd)
                                .arg(lTheorySpd)
                                .arg(rTheorySpd)
                                );
}

bool DataBaseMgr::convertToText(const QStringList &filePaths)
{
    if (filePaths.isEmpty()) {
        return true;
    }

    QProgressDialog progressDialog(QObject::tr("Converting files, Please wait..."),
                                   QObject::tr("Cancel"), 0, filePaths.count() - 1);
    progressDialog.setStyleSheet("QLabel{color:black;}");
    progressDialog.setCancelButton(Q_NULLPTR);
    progressDialog.setAutoClose(false);
    progressDialog.setAutoReset(false);
    progressDialog.show();

    QString targetDir(QApplication::applicationDirPath().append("/../data/conv/"));
    QDir().mkdir(targetDir);

    for (int i = 0; i < filePaths.count(); ++i) {

        QFileInfo fileInfo(filePaths[i]);
        progressDialog.setLabelText(QObject::tr("Converting file '%1' ...").arg(fileInfo.fileName()));
        progressDialog.setValue(i);
        QApplication::processEvents();

        if (!filePaths[i].endsWith(".db") || !this->open(filePaths[i])) {
            continue;
        }

        QFile file(targetDir + fileInfo.baseName().append(".txt"));
        if (!file.open(QFile::WriteOnly)) {
            this->close();
            continue;
        }
        QTextStream stream(&file);
        QSqlQuery query(q_db);
        bool result = query.exec("select * from tbl_recv_data order by msTime");
        if (!result) {
            file.close();
            this->close();
            continue;
        }
        stream << QObject::tr(" * time")
               << QObject::tr(" * lMBrakeP")
               << QObject::tr(" * lABrakeP")
               << QObject::tr(" * rMBrakeP")
               << QObject::tr(" * rABrakeP")
               << QObject::tr(" * lWheelSpd")
               << QObject::tr(" * rWheelSpd")
               << QObject::tr(" * lTheorySpd")
               << QObject::tr(" * rTheorySpd")
               << "\r\n";
        while (query.next()) {
            stream << QDateTime::fromMSecsSinceEpoch(query.value(0).toULongLong()).toString("yyyy-MM-dd HH:mm:ss") << " -> "
                   << query.value(1).toInt() << " "     // lMBrakeP
                   << query.value(2).toInt() << " "     // lABrakeP
                   << query.value(3).toInt() << " "     // rMBrakeP
                   << query.value(4).toInt() << " "     // rABrakeP
                   << query.value(5).toInt() << " "     // lWheelSpd
                   << query.value(6).toInt() << " "     // rWheelSpd
                   << query.value(7).toInt() << " "     // lTheorySpd
                   << query.value(8).toInt() << "\r\n"; // rTheorySpd
        }
        this->close();
    }

    return true;
}

