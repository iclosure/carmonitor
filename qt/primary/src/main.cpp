#include "precomp.h"
#include <QTextCodec>
#include "main_widget.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QTranslator translator;
    translator.load(":/lang/" + QString("zh_CN"));
    app.installTranslator(&translator);

    QFile file(":/qss/stylesheet-1");
    file.open(QFile::ReadOnly);
    app.setStyleSheet(file.readAll());
    app.setFont(QFont("微软雅黑", 12));

    //
    app.setOrganizationDomain("iclosure.com");
    app.setOrganizationName("Smartsoft");
    app.setApplicationName("CarMonitor");
    app.setApplicationDisplayName(QApplication::tr("Car System Monitor"));
    app.setApplicationVersion(QString("%1.%2.%3")
                                   .arg(TARGET_VER_MAJ)
                                   .arg(TARGET_VER_MIN)
                                   .arg(TARGET_VER_PAT));
    //
    MainWidget mainWidget;
    mainWidget.showFullScreen();

    return app.exec();
}
