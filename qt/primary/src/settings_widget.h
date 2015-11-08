#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QDialog>

class SettingsWidget : public QDialog
{
    Q_OBJECT
public:
    explicit SettingsWidget(QWidget *parent = 0);
    ~SettingsWidget();

signals:

public slots:

private slots:
    void stylize();

    // QWidget interface
protected:
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);

private:
    bool q_mousePressed;
    QPoint q_orgPos;
};

#endif // SETTINGSWIDGET_H
