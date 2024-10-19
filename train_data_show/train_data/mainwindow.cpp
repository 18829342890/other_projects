#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFile>
#include <QTextStream>
#include <QWheelEvent>
#include <QMessageBox>
#include <QRegularExpression>
#include <QDateTime>
// #include <QtCharts>

// using namespace QtCharts;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , chart(new QChart)
    , seriesVoltage(new QLineSeries)
    , seriesCurrent(new QLineSeries)
    , seriesEnergy(new QLineSeries)
    , axisX(new QDateTimeAxis)
    , axisY(new QValueAxis)
{
    ui->setupUi(this);
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->viewport()->installEventFilter(this);

    chart->addSeries(seriesVoltage);
    chart->addSeries(seriesCurrent);
    chart->addSeries(seriesEnergy);

    axisX->setFormat("yyyy-MM-dd hh:mm:ss");
    axisX->setTitleText("时间");
    chart->addAxis(axisX, Qt::AlignBottom);
    seriesVoltage->attachAxis(axisX);
    seriesCurrent->attachAxis(axisX);
    seriesEnergy->attachAxis(axisX);

    axisY->setTitleText("数值");
    chart->addAxis(axisY, Qt::AlignLeft);
    seriesVoltage->attachAxis(axisY);
    seriesCurrent->attachAxis(axisY);
    seriesEnergy->attachAxis(axisY);

    setCentralWidget(chartView);

    loadFile("energy.txt");
    plotData();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadFile(const QString &fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, tr("Warning"), tr("Cannot open file: ") + fileName);
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        if (fields.size() < 4) continue;

        qint64 timestamp = fields[0].toLongLong();
        qreal voltage = fields[1].toDouble();
        qreal current = fields[2].toDouble();
        qreal energy = fields[3].toDouble();

        seriesVoltage->append(timestamp * 1000.0, voltage);
        seriesCurrent->append(timestamp * 1000.0, current);
        seriesEnergy->append(timestamp * 1000.0, energy);
    }
}


void MainWindow::plotData() {
    seriesVoltage->setName("电压");
    seriesCurrent->setName("电流");
    seriesEnergy->setName("能耗");

    qreal minX = seriesVoltage->at(0).x();
    qreal maxX = seriesVoltage->at(seriesVoltage->count() - 1).x();
    qreal minY = seriesVoltage->at(0).y();
    qreal maxY = minY;

    for (const QPointF &p : seriesVoltage->points()) {
        minY = qMin(minY, p.y());
        maxY = qMax(maxY, p.y());
    }
    for (const QPointF &p : seriesCurrent->points()) {
        minY = qMin(minY, p.y());
        maxY = qMax(maxY, p.y());
    }
    for (const QPointF &p : seriesEnergy->points()) {
        minY = qMin(minY, p.y());
        maxY = qMax(maxY, p.y());
    }

    axisX->setRange(QDateTime::fromMSecsSinceEpoch(minX), QDateTime::fromMSecsSinceEpoch(maxX));
    axisY->setRange(minY, maxY);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::Wheel) {
        QWheelEvent *wEvent = static_cast<QWheelEvent *>(event);
        wheelEvent(wEvent);
        return true;
    } else {
        return QMainWindow::eventFilter(obj, event);
    }
}

void MainWindow::wheelEvent(QWheelEvent *event) {
    if (event->angleDelta().y() > 0) {
        // 放大
        axisX->setRange(axisX->min().addSecs(-10), axisX->max().addSecs(10));
    } else {
        // 缩小
        axisX->setRange(axisX->min().addSecs(10), axisX->max().addSecs(-10));
    }
}
