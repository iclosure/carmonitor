#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QPushButton>
#include "protocol/protocol.h"

///! # JSwitchButton #

class JSwitchButton : public QPushButton
{
    Q_OBJECT
public:
    explicit JSwitchButton(QWidget *parent = 0);
    explicit JSwitchButton(const QPixmap &pixmap, QWidget *parent = 0);

    void setSize(int w, int h);

signals:
    void stateChanged(bool);

public slots:
    void setState(bool state);

protected:
    void paintEvent(QPaintEvent *);

private:
    bool q_isOn;
    QPixmap q_pixmap;
};

///! # JTracksipButton #

class JTracksipButton : public QPushButton
{
    Q_OBJECT
public:
    explicit JTracksipButton(QWidget *parent = 0);
    explicit JTracksipButton(const QPixmap &pixmap, QWidget *parent = 0);

    void setSize(int w, int h);

signals:
    void stateChanged(bool);

public slots:
    void setState(bool state);

protected:
    void paintEvent(QPaintEvent *);
    //bool event(QEvent *e);

private:
    bool q_isOn;
    QPixmap q_pixmap;
};

///! # JPushButton #

class JPushButton : public QPushButton
{
    Q_OBJECT
public:
    explicit JPushButton(QWidget *parent = 0);
    explicit JPushButton(const QString &text, QWidget *parent = 0);
};

///! # JButtonDash #

class JButtonDash : public QPushButton
{
    Q_OBJECT
public:
    explicit JButtonDash(QWidget *parent = 0);
    explicit JButtonDash(const QString &text, QWidget *parent = 0);
};

///! # MainWidget #

class MainWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MainWidget(QWidget *parent = 0);

protected:
    void showEvent(QShowEvent *event);
    void closeEvent(QCloseEvent *event);

public slots:
    void execSliderWidget();

private:
    void readSettings();
    void writeSettings();

private:
    SerialPortSend q_sendData;
    quint32 q_lTheorySpd;
    quint32 q_rTheorySpd;
};

#endif // MAINWIDGET_H
