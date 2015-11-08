#ifndef HISTORYWIDGET_H
#define HISTORYWIDGET_H

#include <QDialog>

class HistoryWidget : public QDialog
{
    Q_OBJECT
public:
    explicit HistoryWidget(QWidget *parent = 0);
    ~HistoryWidget();

    static QColor randomColor(int index);

private:
};

#endif // HISTORYWIDGET_H
