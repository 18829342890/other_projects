#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>

// using namespace QtCharts;
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    // 加载路径中的文件数据
    void on_pushButtonLoadFile_clicked();

private:
    void loadFile(const QString& fileName);
    void plotData();

private:
    Ui::MainWindow *ui;
    QChart *chart;
    QLineSeries *seriesVoltage;
    QLineSeries *seriesCurrent;
    QLineSeries *seriesEnergy;
    QDateTimeAxis *axisX;
    QValueAxis *axisY;
    // QPointF lastMousePos;
    bool isDragging;
    int64_t dragStartX;
    int64_t dragStartMinValue;
    int64_t dragStartMaxValue;

};
#endif // MAINWINDOW_H
