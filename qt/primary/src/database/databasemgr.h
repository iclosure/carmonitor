#ifndef DATABASEMGR_H
#define DATABASEMGR_H

#include <QObject>
#include <QSqlDatabase>
#include <QDateTime>
#include "../protocol/serialport_proxy.h"

class DataBaseMgr
{
public:
    DataBaseMgr();
    ~DataBaseMgr();

    static DataBaseMgr &instance();

    bool create(const QString &name);
    bool open(const QString &name = QString());
    void close();
    void save();
    void restore();

    quint64 startTime() const;
    quint64 endTime() const;

    bool read(const QString &section, QVector<QPointF> &points,
              const quint64 &startTime = 0, const quint64 &endTime = 0);
    bool write(const SerialPortRecv &data, quint32 lTheorySpd, quint32 rTheorySpd,
               const quint64 &time, bool async = false);

    bool convertToText(const QStringList &filePaths);

private:
    QSqlDatabase q_db;
    QString q_name;
    bool q_isOpen;
};

#endif // DATABASEMGR_H
