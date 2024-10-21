#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFile>
#include <QTextStream>
#include <QWheelEvent>
#include <QMessageBox>
#include <QRegularExpression>
#include <QDateTime>
#include <QFileDialog>
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>
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
    , isDragging(false)
    , dragStartX(0)
    , dragStartMinValue(0)
    , dragStartMaxValue(0)
{
    ui->setupUi(this);

    // 设置布局
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->viewport()->installEventFilter(this);
    chartView->setMouseTracking(true);

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

    // 连接按钮点击信号到槽函数
    connect(ui->openFileButton, &QPushButton::clicked, this, &MainWindow::on_pushButtonLoadFile_clicked);
    ui->toolBar->addWidget(ui->openFileButton);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButtonLoadFile_clicked() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Text Files (*.txt)"));
    if (!fileName.isEmpty()) {
        // ui->lineEditFilePath->setText(fileName); // 在文本行编辑中显示选择的文件路径
        loadFile(fileName);
        plotData();
    }
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
    } else if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        mousePressEvent(mouseEvent);
        return true;
    } else if (event->type() == QEvent::MouseMove) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        mouseMoveEvent(mouseEvent);
        return true;
    } else if (event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        mouseReleaseEvent(mouseEvent);
        return true;
    } else {
        return QMainWindow::eventFilter(obj, event);
    }
}

void MainWindow::wheelEvent(QWheelEvent *event) {
    double factor = qPow(1.0015, event->angleDelta().y());

    double xRange = axisX->max().toMSecsSinceEpoch() - axisX->min().toMSecsSinceEpoch();
    double x = event->position().x() / width() * xRange + axisX->min().toMSecsSinceEpoch();

    double minX = x - (x - axisX->min().toMSecsSinceEpoch()) * factor;
    double maxX = x + (axisX->max().toMSecsSinceEpoch() - x) * factor;

    axisX->setRange(QDateTime::fromMSecsSinceEpoch(minX), QDateTime::fromMSecsSinceEpoch(maxX));
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        isDragging = true;
        dragStartX = event->pos().x();
        dragStartMinValue = axisX->min().toMSecsSinceEpoch();
        dragStartMaxValue = axisX->max().toMSecsSinceEpoch();
    }
    QMainWindow::mousePressEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    if (isDragging) {
        int deltaX = event->pos().x() - dragStartX;
        qint64 xRange = dragStartMaxValue - dragStartMinValue;
        qint64 movePixelsToMs = xRange / width(); // 每像素代表的时间毫秒数
        qint64 newMinValue = dragStartMinValue + deltaX * movePixelsToMs;
        qint64 newMaxValue = dragStartMaxValue + deltaX * movePixelsToMs;

        // 防止越界
        if (newMinValue < seriesVoltage->at(0).x()) {
            newMinValue = seriesVoltage->at(0).x();
            newMaxValue = newMinValue + xRange;
        } else if (newMaxValue > seriesVoltage->at(seriesVoltage->count() - 1).x()) {
            newMaxValue = seriesVoltage->at(seriesVoltage->count() - 1).x();
            newMinValue = newMaxValue - xRange;
        }

        axisX->setRange(QDateTime::fromMSecsSinceEpoch(newMinValue), QDateTime::fromMSecsSinceEpoch(newMaxValue));
    }
    QMainWindow::mouseMoveEvent(event);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        isDragging = false;
    }
    QMainWindow::mouseReleaseEvent(event);
}

