#include "precomp.h"
#include "history_widget.h"
#include "curve_widget.h"
#include "database/databasemgr.h"

static const QColor _v_curve_colors[8] = {
    Qt::white,
    Qt::gray,
    Qt::red,
    Qt::green,
    Qt::blue,
    Qt::cyan,
    Qt::magenta,
    Qt::darkYellow
};

static bool _v_curve_checked[8] = {true, true, true, true, true, true, true, true};

HistoryWidget::HistoryWidget(QWidget *parent)
    : QDialog(parent)
{
    setObjectName("HistoryWidget");
    setWindowTitle(tr("History Data View"));

    // layout - top
    QHBoxLayout *horiLayoutTop = new QHBoxLayout();

    QPushButton *buttonQuit = new QPushButton(this);
    buttonQuit->setObjectName("buttonQuit");
    horiLayoutTop->addSpacing(25);
    horiLayoutTop->addWidget(buttonQuit, 0, Qt::AlignLeft);
    horiLayoutTop->addStretch();

    // button-export
    QPushButton *buttonExport = new QPushButton(this);
    buttonExport->setObjectName("buttonExport");
    horiLayoutTop->addWidget(buttonExport);
    horiLayoutTop->addStretch();

    // button-open
    QPushButton *buttonOpen = new QPushButton(this);
    buttonOpen->setObjectName("buttonOpen");
    horiLayoutTop->addWidget(buttonOpen);
    horiLayoutTop->addStretch();

    QFormLayout *formLayoutTime = new QFormLayout();
    formLayoutTime->setFormAlignment(Qt::AlignVCenter);
    horiLayoutTop->addLayout(formLayoutTime);
    horiLayoutTop->addStretch();

    QDateTimeEdit *dateTimeEditStart = new QDateTimeEdit(this);
    dateTimeEditStart->setObjectName("dateTimeEditStart");
    dateTimeEditStart->setDisplayFormat("yyyy-MM-dd HH:mm:ss");
    formLayoutTime->addRow(tr("Start Time:"), dateTimeEditStart);

    QDateTimeEdit *dateTimeEditEnd = new QDateTimeEdit(this);
    dateTimeEditEnd->setObjectName("dateTimeEditEnd");
    dateTimeEditEnd->setDisplayFormat("yyyy-MM-dd HH:mm:ss");
    formLayoutTime->addRow(tr("End Time:"), dateTimeEditEnd);

    //LBP
    QFormLayout *formLayoutLBP = new QFormLayout();
    formLayoutLBP->setFormAlignment(Qt::AlignVCenter);
    formLayoutLBP->setLabelAlignment(Qt::AlignRight);
    horiLayoutTop->addLayout(formLayoutLBP);
    horiLayoutTop->addStretch();
    QCheckBox *checkBoxLBPMajor = new QCheckBox(tr("Major"), this);
    checkBoxLBPMajor->setProperty("curveColor", "#101010");
    QCheckBox *checkBoxLBPMinor = new QCheckBox(tr("Minor"), this);
    checkBoxLBPMinor->setProperty("curveColor", "#101010");
    formLayoutLBP->addRow(tr("LBP:"), checkBoxLBPMajor);
    formLayoutLBP->addRow("", checkBoxLBPMinor);

    //RBP
    QFormLayout *formLayoutRBP = new QFormLayout();
    formLayoutRBP->setFormAlignment(Qt::AlignVCenter);
    formLayoutRBP->setLabelAlignment(Qt::AlignRight);
    horiLayoutTop->addLayout(formLayoutRBP);
    horiLayoutTop->addStretch();
    QCheckBox *checkBoxRBPMajor = new QCheckBox(tr("Major"), this);
    checkBoxRBPMajor->setProperty("curveColor", "#101010");
    QCheckBox *checkBoxRBPMinor = new QCheckBox(tr("Minor"), this);
    checkBoxRBPMinor->setProperty("curveColor", "#101010");
    formLayoutRBP->addRow(tr("RBP:"), checkBoxRBPMajor);
    formLayoutRBP->addRow("", checkBoxRBPMinor);

    //LRP
    QFormLayout *formLayoutLRP = new QFormLayout();
    formLayoutLRP->setFormAlignment(Qt::AlignVCenter);
    formLayoutLRP->setLabelAlignment(Qt::AlignRight);
    horiLayoutTop->addLayout(formLayoutLRP);
    horiLayoutTop->addStretch();
    QCheckBox *checkBoxLRPTheory = new QCheckBox(tr("Theory"), this);
    checkBoxLRPTheory->setProperty("curveColor", "#101010");
    QCheckBox *checkBoxLRPReal = new QCheckBox(tr("Real"), this);
    checkBoxLRPReal->setProperty("curveColor", "#101010");
    formLayoutLRP->addRow(tr("LRP:"), checkBoxLRPTheory);
    formLayoutLRP->addRow("", checkBoxLRPReal);

    //RRP
    QFormLayout *formLayoutRRP = new QFormLayout();
    formLayoutRRP->setFormAlignment(Qt::AlignVCenter);
    formLayoutRRP->setLabelAlignment(Qt::AlignRight);
    horiLayoutTop->addLayout(formLayoutRRP);
    horiLayoutTop->addStretch();
    QCheckBox *checkBoxRRPTheory = new QCheckBox(tr("Theory"), this);
    checkBoxRRPTheory->setProperty("curveColor", "#101010");
    QCheckBox *checkBoxRRPReal = new QCheckBox(tr("Real"), this);
    checkBoxRRPReal->setProperty("curveColor", "#101010");
    formLayoutRRP->addRow(tr("RRP:"), checkBoxRRPTheory);
    formLayoutRRP->addRow("", checkBoxRRPReal);

    // button-update
    QPushButton *buttonUpdate = new QPushButton(this);
    buttonUpdate->setObjectName("buttonUpdate");
    horiLayoutTop->addWidget(buttonUpdate);
    horiLayoutTop->addStretch();
/*
    // button-undo
    QPushButton *buttonUndo = new QPushButton(this);
    buttonUndo->setObjectName("buttonUndo");
    horiLayoutTop->addWidget(buttonUndo);
    horiLayoutTop->addStretch();
*/
    // middle - curves
    CurveWidget* curveHistory = new CurveWidget(tr("History Data View"), this, true);
    curveHistory->setMaximumWidth(10000000);
    curveHistory->setScaleLabelFormat("yyyy/MM/dd\n  HH:mm:ss");
    curveHistory->clear();

    QVBoxLayout *vertLayoutMain = new QVBoxLayout(this);
    vertLayoutMain->addLayout(horiLayoutTop);
    vertLayoutMain->addWidget(curveHistory);

    connect(buttonQuit, &QPushButton::clicked, this, [=](){
        this->accept();
    });

    connect(buttonOpen, &QPushButton::clicked, this, [=](){
        QFileDialog fileDialog(this,
                               tr("Open history database files"),
                               QApplication::applicationDirPath().append("/../data"),
                               tr("Data Base File (*.db *.mdb)"));
        if (fileDialog.exec() == QDialog::Rejected) {
            return;
        }

        // clear curve
        curveHistory->clear();

        //
        QStringList filePaths = fileDialog.selectedFiles();
        if (filePaths.isEmpty()) {
            return;
        }

        QString filePath = filePaths.first();
        if (filePath.isEmpty()) {
            return;
        }

        // open database
        if (!DataBaseMgr::instance().open(filePath)) {
            Q_ASSERT(false);
            return;
        }

        //
        QDateTime startTime = QDateTime::fromMSecsSinceEpoch(DataBaseMgr::instance().startTime());
        QDateTime endTime = QDateTime::fromMSecsSinceEpoch(DataBaseMgr::instance().endTime());
        dateTimeEditStart->setDateTimeRange(startTime, endTime);
        dateTimeEditEnd->setDateTimeRange(startTime, endTime);
        dateTimeEditEnd->setDateTime(endTime);
        dateTimeEditStart->setDateTime(startTime);

        // curve title
        curveHistory->setTitle(tr("History Data View")
                               .append(" (")
                               .append(QFileInfo(filePath).fileName())
                               .append(")"));
    });
    connect(buttonExport, &QPushButton::clicked, this, [=](){
        QStringList filePaths = QFileDialog::getOpenFileNames(this,
                                                              tr("Convert data base files to txt format and save..."),
                                                              QApplication::applicationDirPath().append("/../data"),
                                                              tr("Data Base File (*.db *.mdb)"));
        if (filePaths.isEmpty()) {
            return;
        }

        //
        if (DataBaseMgr::instance().convertToText(filePaths)) {
            QMessageBox::information(this, tr("Convert Database file"), tr("Convert successfully!"));
        } else {
            QMessageBox::information(this, tr("Convert Database file"), tr("Convert failed!"));
        }
    });

    connect(dateTimeEditStart, &QDateTimeEdit::dateTimeChanged, this, [=](const QDateTime &dateTime){
        QDateTime endDateTime = dateTimeEditEnd->dateTime();
        if (dateTime > endDateTime) {
            dateTimeEditStart->setDateTime(endDateTime);
        }
    });

    connect(dateTimeEditEnd, &QDateTimeEdit::dateTimeChanged, this, [=](const QDateTime &dateTime){
        QDateTime startDateTime = dateTimeEditStart->dateTime();
        if (dateTime < startDateTime) {
            dateTimeEditEnd->setDateTime(startDateTime);
        }
    });

    connect(checkBoxLBPMajor, &QCheckBox::toggled, this, [=](bool checked){
        _v_curve_checked[0] = checked;
    });
    connect(checkBoxLBPMinor, &QCheckBox::toggled, this, [=](bool checked){
        _v_curve_checked[1] = checked;
    });
    connect(checkBoxRBPMajor, &QCheckBox::toggled, this, [=](bool checked){
        _v_curve_checked[2] = checked;
    });
    connect(checkBoxRBPMinor, &QCheckBox::toggled, this, [=](bool checked){
        _v_curve_checked[3] = checked;
    });
    connect(checkBoxLRPTheory, &QCheckBox::toggled, this, [=](bool checked){
        _v_curve_checked[4] = checked;
    });
    connect(checkBoxLRPReal, &QCheckBox::toggled, this, [=](bool checked){
        _v_curve_checked[5] = checked;
    });
    connect(checkBoxRRPTheory, &QCheckBox::toggled, this, [=](bool checked){
        _v_curve_checked[6] = checked;
    });
    connect(checkBoxRRPReal, &QCheckBox::toggled, this, [=](bool checked){
        _v_curve_checked[7] = checked;
    });

    connect(buttonUpdate, &QPushButton::clicked, this, [=](){
        // clear curve
        curveHistory->clear();

        QVector<QPointF> points;
        quint64 startTime = dateTimeEditStart->dateTime().toMSecsSinceEpoch();
        quint64 endTime = dateTimeEditEnd->dateTime().toMSecsSinceEpoch();

        // LBP-Major
        if (checkBoxLBPMajor->isChecked()) {
            // read
            points.clear();
            if (DataBaseMgr::instance().read("lMBrakeP", points, startTime, endTime)) {
                curveHistory->addCurve(tr("LBP-Major"), QPen(randomColor(0)), points);
            }
        }

        //QApplication::processEvents();

        // LBP-Minor
        if (checkBoxLBPMinor->isChecked()) {
            // read
            points.clear();
            if (DataBaseMgr::instance().read("lABrakeP", points, startTime, endTime)) {
                curveHistory->addCurve(tr("LBP-Minor"), QPen(randomColor(1)), points);
            }
        }

        //QApplication::processEvents();

        // RBP-Major
        if (checkBoxRBPMajor->isChecked()) {
            // read
            points.clear();
            if (DataBaseMgr::instance().read("rMBrakeP", points, startTime, endTime)) {
                curveHistory->addCurve(tr("RBP-Major"), QPen(randomColor(2)), points);
            }
        }

        //QApplication::processEvents();

        // RBP-Minor
        if (checkBoxRBPMinor->isChecked()) {
            // read
            points.clear();
            if (DataBaseMgr::instance().read("rABrakeP", points, startTime, endTime)) {
                curveHistory->addCurve(tr("RBP-Minor"), QPen(randomColor(3)), points);
            }
        }

        //QApplication::processEvents();

        // LRP-Theory
        if (checkBoxLRPTheory->isChecked()) {
            // read
            points.clear();
            if (DataBaseMgr::instance().read("lTheorySpd", points, startTime, endTime)) {
                curveHistory->addCurve(tr("LRP-Theory"), QPen(randomColor(4)), points);
            }
        }

        //QApplication::processEvents();

        // LRP-Real
        if (checkBoxLRPReal->isChecked()) {
            // read
            points.clear();
            if (DataBaseMgr::instance().read("lWheelSpd", points, startTime, endTime)) {
                curveHistory->addCurve(tr("LRP-Real"), QPen(randomColor(5)), points);
            }
        }

        //QApplication::processEvents();

        // RRP-Theory
        if (checkBoxRRPTheory->isChecked()) {
            // read
            points.clear();
            if (DataBaseMgr::instance().read("rTheorySpd", points, startTime, endTime)) {
                curveHistory->addCurve(tr("RRP-Theory"), QPen(randomColor(6)), points);
            }
        }

        //QApplication::processEvents();

        // RRP-Real
        if (checkBoxRRPReal->isChecked()) {
            // read
            points.clear();
            if (DataBaseMgr::instance().read("rWheelSpd", points, startTime, endTime)) {
                curveHistory->addCurve(tr("RRP-Real"), QPen(randomColor(7)), points);
            }
        }
    });
/*
    connect(buttonUndo, &QPushButton::clicked, this, [=](){
        curveHistory->setNormalScale();
    });
*/
    // finally initialize
    checkBoxLBPMajor->setChecked(_v_curve_checked[0]);
    checkBoxLBPMinor->setChecked(_v_curve_checked[1]);
    checkBoxRBPMajor->setChecked(_v_curve_checked[2]);
    checkBoxRBPMinor->setChecked(_v_curve_checked[3]);
    checkBoxLRPTheory->setChecked(_v_curve_checked[4]);
    checkBoxLRPReal->setChecked(_v_curve_checked[5]);
    checkBoxRRPTheory->setChecked(_v_curve_checked[6]);
    checkBoxRRPReal->setChecked(_v_curve_checked[7]);
}

HistoryWidget::~HistoryWidget()
{

}

QColor HistoryWidget::randomColor(int index)
{
    return _v_curve_colors[index];//QColor((qrand() % 150) + 64, (qrand() % 150) + 64, (qrand() % 150) + 64);
}

