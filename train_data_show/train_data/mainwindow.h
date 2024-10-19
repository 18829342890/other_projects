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

private slots:
    // 加载路径中的文件数据
    void onLoadFileButtonClicked();


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
};
#endif // MAINWINDOW_H
