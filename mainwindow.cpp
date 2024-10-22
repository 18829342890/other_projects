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
    , dataFilePath("")
    , samplingPrecision(maxInterval)
    , iuWheelEvent(false)
    , dataLoader(new DataLoader(this, seriesVoltage, seriesCurrent, seriesEnergy, this))
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
    connect(dataLoader, &DataLoader::dataLoaded, this, &MainWindow::on_dataLoaded);
    ui->toolBar->addWidget(ui->openFileButton);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButtonLoadFile_clicked() {
    // QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Text Files (*.txt)"));
    // if (!fileName.isEmpty()) {
    //     dataFilePath = fileName;
    //     loadFile(fileName);
    //     plotData();
    // }
    qInfo() << "inter on_pushButtonLoadFile_clicked...";
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Text Files (*.txt)"));
    if (!fileName.isEmpty()) {
        dataFilePath = fileName;
        dataLoader->loadData(dataFilePath);
    }
}

void MainWindow::on_dataLoaded() {
    plotData();
}

// void MainWindow::loadFile(const QString &fileName) {
//     QFile file(fileName);
//     if (!file.open(QIODevice::ReadOnly)) {
//         QMessageBox::warning(this, tr("Warning"), tr("Cannot open file: ") + fileName);
//         return;
//     }

//     // 初始化各个精度的上一个时间戳
//     QMap<qint64, qint64> lastTimestamps;
//     qint64 precision = maxInterval;
//     while (precision  > 500) {
//         qInfo() << "precision:" << precision;
//         lastTimestamps[precision] = 0;
//         precision /= 2;
//     }
//     // for (qint64 precision : {500, 1000, 2000, 5000, 10000}) {
//     //     lastTimestamps[precision] = 0;
//     // }

//     QTextStream in(&file);
//     // qint64 count = 0;
//     while (!in.atEnd()) {
//         QString line = in.readLine();
//         QStringList fields = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
//         if (fields.size() < 4) continue;

//         qint64 timestamp = fields[0].toLongLong(); // 毫秒
//         qreal voltage = fields[1].toDouble();
//         qreal current = fields[2].toDouble();
//         qreal energy = fields[3].toDouble();

//         QPointF pointVoltage(timestamp * 1.0, voltage);
//         QPointF pointCurrent(timestamp * 1.0, current);
//         QPointF pointEnergy(timestamp * 1.0, energy);
//         // count += 1;
//         // if (count >= 50) break;

//         precision = maxInterval;
//         while (precision > 500) {
//         // for (qint64 precision : {500, 1000, 2000, 5000, 10000}) {
//             // qInfo() << "precision:" << precision << ", timestamp:" << timestamp << ", lastTimestamp:" << lastTimestamps[precision]
//             //         << ", timestamp - lastTimestamps[precision] = " << timestamp - lastTimestamps[precision];
//             if (timestamp - lastTimestamps[precision] >= precision){
//                 preprocessedDataVoltage[precision].append(pointVoltage);
//                 preprocessedDataCurrent[precision].append(pointCurrent);
//                 preprocessedDataEnergy[precision].append(pointEnergy);
//                 lastTimestamps[precision] = timestamp;
//                 // qInfo() << "precision[" << precision << "] preprocessedDataVoltage size:" << preprocessedDataVoltage[precision].size()
//                 //         << ", preprocessedDataCurrent size:" << preprocessedDataCurrent[precision].size()
//                 //         << ", preprocessedDataEnergy size:" << preprocessedDataEnergy[precision].size();
//             }
//             precision /= 2;
//         }
//         // seriesVoltage->append(timestamp * 1.0, voltage);
//         // seriesCurrent->append(timestamp * 1.0, current);
//         // seriesEnergy->append(timestamp * 1.0, energy);
//     }

//     precision = maxInterval;
//     while (precision> 500) {
//         qInfo() << "currentPrecision[" << precision << "] preprocessedDataVoltage size:" << preprocessedDataVoltage[precision].size()
//         << ", preprocessedDataCurrent size:" << preprocessedDataCurrent[precision].size()
//         << ", preprocessedDataEnergy size:" << preprocessedDataEnergy[precision].size();
//         precision -= 2000;
//     }
//     // for (qint64 precision : {500, 1000, 2000, 5000, 10000}) {
//     //     qInfo() << "precision[" << precision << "] preprocessedDataVoltage size:" << preprocessedDataVoltage[precision].size()
//     //             << ", preprocessedDataCurrent size:" << preprocessedDataCurrent[precision].size()
//     //             << ", preprocessedDataEnergy size:" << preprocessedDataEnergy[precision].size();
//     // }
// }


// void MainWindow::plotData() {
//     // 清空之前的数据，插入精度值对于的数据集
//     qInfo() << "plotData samplingPrecision:" << samplingPrecision << ", preprocessedDataVoltage size:" << preprocessedDataVoltage[samplingPrecision].size()
//             << ", preprocessedDataCurrent size:" << preprocessedDataCurrent[samplingPrecision].size()
//             << ", preprocessedDataEnergy size:" << preprocessedDataEnergy[samplingPrecision].size();
//     seriesVoltage->clear();
//     seriesVoltage->append(preprocessedDataVoltage[samplingPrecision]);
//     seriesCurrent->clear();
//     seriesCurrent->append(preprocessedDataCurrent[samplingPrecision]);
//     seriesEnergy->clear();
//     seriesEnergy->append(preprocessedDataEnergy[samplingPrecision]);

//     seriesVoltage->setName("电压");
//     seriesCurrent->setName("电流");
//     seriesEnergy->setName("能耗");

//     // Y轴的最大最小值：左右各移四分之一，一共只展示原来的二分之一时间范围
//     qreal minX = seriesVoltage->at(0).x();
//     qreal maxX = seriesVoltage->at(seriesVoltage->count() - 1).x();
//     if (iuWheelEvent) {
//         minX = axisX->min().toMSecsSinceEpoch();
//         maxX = axisX->max().toMSecsSinceEpoch();
//         qreal xLength = maxX - minX;
//         qreal oneFourthXLength = xLength / 4;
//         qInfo() << "minX:"<< static_cast<long long>(minX) << ", maxX:" << static_cast<long long>(maxX) << ", xLength:" << static_cast<long long>(xLength) << ", oneFourthXLength:" << static_cast<long long>(oneFourthXLength);
//         minX = minX + oneFourthXLength;
//         maxX = maxX - oneFourthXLength;
//     }
//     qInfo() << "end minX:"<< static_cast<long long>(minX) << ",end maxX:" <<static_cast<long long>(maxX);

//     // Y轴的最大最小值
//     qreal minY = seriesVoltage->at(0).y();
//     qreal maxY = minY;
//     for (const QPointF &p : seriesVoltage->points()) {
//         minY = qMin(minY, p.y());
//         maxY = qMax(maxY, p.y());
//     }
//     for (const QPointF &p : seriesCurrent->points()) {
//         minY = qMin(minY, p.y());
//         maxY = qMax(maxY, p.y());
//     }
//     for (const QPointF &p : seriesEnergy->points()) {
//         minY = qMin(minY, p.y());
//         maxY = qMax(maxY, p.y());
//     }
//     qInfo() << "end minY:"<< static_cast<long long>(minY) << ",end maxY:" <<static_cast<long long>(maxY);

//     axisX->setRange(QDateTime::fromMSecsSinceEpoch(minX), QDateTime::fromMSecsSinceEpoch(maxX));
//     axisY->setRange(minY, maxY);
// }

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
    qInfo() << "plotData end minY:"<< static_cast<long long>(minY) << ",end maxY:" <<static_cast<long long>(maxY);
    qInfo() << "plotData seriesVoltage size:" << seriesVoltage->count() << ", seriesCurrent size:" << seriesCurrent->count() << ", seriesEnergy:" << seriesEnergy->count();

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

// void MainWindow::wheelEvent(QWheelEvent *event) {
//     iuWheelEvent = true;
//     double factor = qPow(1.0015, event->angleDelta().y());
//     if (factor > 1 && samplingPrecision > 500) {
//         // 放大，抽样精度变小，并重新加载和绘制数据
//         samplingPrecision /= 2;
//         qInfo() << "fangda samplingPrecision:"  << samplingPrecision;

//     } else if (factor < 1 && samplingPrecision < 60000) {
//         // 缩小，抽样精度变大，并重新加载和绘制数据
//         samplingPrecision *= 2;
//         qInfo() << "suoxiao samplingPrecision:"  << samplingPrecision;
//     }

//     // 重新绘制数据
//     plotData();

//     // double xRange = axisX->max().toMSecsSinceEpoch() - axisX->min().toMSecsSinceEpoch();
//     // double x = event->position().x() / width() * xRange + axisX->min().toMSecsSinceEpoch();

//     // double minX = x - (x - axisX->min().toMSecsSinceEpoch()) * factor;
//     // double maxX = x + (axisX->max().toMSecsSinceEpoch() - x) * factor;

//     // axisX->setRange(QDateTime::fromMSecsSinceEpoch(minX), QDateTime::fromMSecsSinceEpoch(maxX));
// }

void MainWindow::wheelEvent(QWheelEvent *event) {
    iuWheelEvent = true;
    double scaleFactor = qPow(1.2, event->angleDelta().y() / 8.0 / 15.0); // 调整缩放因子

    // 更新抽样精度
    samplingPrecision = qBound(500, static_cast<int>(samplingPrecision * scaleFactor), 60000);

    // 获取当前X轴范围
    qint64 currentMin = axisX->min().toMSecsSinceEpoch();
    qint64 currentMax = axisX->max().toMSecsSinceEpoch();
    qint64 range = currentMax - currentMin;

    // 计算新的X轴范围
    qint64 newMin = currentMin + (1 - scaleFactor) * range / 2;
    qint64 newMax = currentMax - (1 - scaleFactor) * range / 2;

    // 限制新范围不超出数据边界
    if (seriesVoltage->count() > 0) {
        qreal firstX = seriesVoltage->at(0).x();
        qreal lastX = seriesVoltage->at(seriesVoltage->count() - 1).x();
        newMin = qBound(firstX, static_cast<qreal>(newMin), lastX);
        newMax = qBound(firstX, static_cast<qreal>(newMax), lastX);
    }

    // 设置新的X轴范围
    axisX->setRange(QDateTime::fromMSecsSinceEpoch(newMin), QDateTime::fromMSecsSinceEpoch(newMax));

    // 重新加载数据以适应新的抽样精度
    dataLoader->setZoomFactor(samplingPrecision);
    dataLoader->loadData(dataFilePath);

    event->accept();
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

DataLoader::DataLoader(MainWindow *mainWin, QLineSeries *seriesVoltage, QLineSeries *seriesCurrent, QLineSeries *seriesEnergy, QObject *parent)
    : QThread(parent)
    , mainWindow(mainWin)
    , seriesVoltage(seriesVoltage)
    , seriesCurrent(seriesCurrent)
    , seriesEnergy(seriesEnergy)
    , zoomFactor(1) {
    seriesVoltage->clear();
    seriesCurrent->clear();
    seriesCurrent->clear();

}

void DataLoader::setZoomFactor(int factor) {
    zoomFactor = factor;
}

void DataLoader::loadData(const QString& fileName) {
    qInfo() << "inter DataLoader::loadData...";
    this->fileName = fileName;
    start();
}

void DataLoader::run() {
    qInfo() << "inter DataLoader::run...";
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(mainWindow, tr("Warning"), tr("Cannot open file: ") + fileName);
        return;
    }

    qint64 fileSize = file.size();
    qint64 dataCount = mainWindow->width();
    qint64 newSamplingPrecisio = zoomFactor * 36;
    qint64 interval = fileSize / dataCount / newSamplingPrecisio * newSamplingPrecisio;
    qInfo() << "fileSize:" << fileSize << ", dataCount:" << dataCount << ", interval:" << interval;

    QTextStream in(&file);
    in.seek(0);
    for (qint64 i = 0; i < dataCount; ++i) {
        QString line = in.readLine();
        QStringList fields = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        if (fields.size() < 4) continue;

        // qint64 timestamp = fields[0].toLongLong(); // 毫秒
        // qreal voltage = fields[1].toDouble();
        // qreal current = fields[2].toDouble();
        // qreal energy = fields[3].toDouble();

        // seriesVoltage->append(timestamp * 1.0, voltage);
        // seriesCurrent->append(timestamp * 1.0, current);
        // seriesEnergy->append(timestamp * 1.0, energy);

        qint64 timestamp = fields[0].toLongLong(); // 毫秒
        qreal voltage = fields[1].toDouble();
        qreal current = fields[2].toDouble();
        qreal energy = fields[3].toDouble();
        qInfo() << "timestamp:" << timestamp << ", voltage:" << voltage << ", current:" << current << ", energy:" << energy;

        seriesVoltage->append(timestamp * 1.0, voltage);
        seriesCurrent->append(timestamp * 1.0, current);
        seriesEnergy->append(timestamp * 1.0, energy);

        in.seek(i * interval);
    }
    qInfo() << "seriesVoltage size:" << seriesVoltage->count() << ", seriesCurrent size:" << seriesCurrent->count() << ", seriesEnergy:" << seriesEnergy->count();

    emit dataLoaded();
}
