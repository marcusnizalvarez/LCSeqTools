#ifndef STATSWINDOW_H
#define STATSWINDOW_H

#include <QWidget>
#include <QtCharts>
#include <QVector>
#include "lcseqtools_cli.h"

using namespace QtCharts;

namespace Ui {
class StatsWindow;
}

class StatsWindow : public QWidget
{
    Q_OBJECT
public:
    explicit StatsWindow(QWidget *parent, LCSeqTools *P_Core);
    ~StatsWindow();
    void ShowDepthPlot();
    void ShowQualityPlot();
    void ShowSubstitutionPlot();
    bool IsValid();
private slots:
    void on_DepthPlotButton_clicked();
    void on_QualityPlotButton_clicked();
    void on_SubstitutionsButton_clicked();
    void Identify1();
    void Identify2();
private:
    void SetChart(QChart* Chart);
    QChartView *ChartView = nullptr;
    Ui::StatsWindow *ui;
    LCSeqTools *Core;
    SnpStatsStruct SnpStats;
    double FindMedian(const QVector<double>& tmpVector, int begin, int end);
    double XLimThreshold_DepthPct=0.001;
    double XLimThreshold_DepthMax=10;
    double XLimThreshold_QualPct=0.001;
    double XLimThreshold_QualMax=50;
};

#endif // STATSWINDOW_H
