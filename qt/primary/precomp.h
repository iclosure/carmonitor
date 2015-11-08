#ifndef PRECOMP_H
#define PRECOMP_H

#include <QtCore>
#include <QtGui>

#ifdef QT_WIDGETS_LIB
#include <QtWidgets>
#endif

#ifdef QT_WINEXTRAS_LIB
#include <QtWinExtras>
#endif

#ifdef QT_SERIALPORT_LIB
#include <QtSerialPort>
#endif

#ifdef QT_QUICK_LIB
#include <QtQuick>
#endif

#ifdef QT_WEBKIT_LIB
#include <QtWebKit>
#endif

#ifdef QT_WEBKITWIDGETS_LIB
#include <QtWebKitWidgets>
#endif

#include <QtSql/QtSql>
#ifdef QT_QTSQL_LIB
//#include <QtSql>
#endif

//
#ifdef QWT_DLL_UNDEF
#endif
#undef QWT_DLL  // static


///////////////////////////////////////////////////////////////

// properity - QApplication

template<typename T>
inline T qAppSetProperity(T t, QString name = QString()) {
    Q_ASSERT(t && t->inherits("QObject"));
    qApp->setProperty((name.isEmpty() ? t->objectName() : name).toStdString().c_str(), QVariant::fromValue<T>(t));
    return t;
}

template<typename T>
inline T qAppProperity(const char* name) {
    Q_ASSERT(name);
    return qApp->property(name).value<T>();
}

inline bool qAppProperityIsValid(const char* name) {
    Q_ASSERT(name);
    return qApp->property(name).isValid();
}

// properity - QObject

template<typename T>
inline bool qObjectSetProperity(QObject *o, T t, const char* name) {
    Q_ASSERT(o && t && name);
    o->setProperty(name, QVariant::fromValue<T>(t));
    return t;
}

template<typename T>
inline T qObjectProperity(QObject *o, const char* name) {
    Q_ASSERT(o && name);
    return o->property(name).value<T>();
}

inline bool qObjectProperityIsValid(QObject *o, const char* name) {
    Q_ASSERT(o && name);
    return o->property(name).isValid();
}

#endif // PRECOMP_H
