#include "statswindow.h"
#include "ui_statswindow.h"
bool StatsWindow::IsValid(){
    return ChartView;
}
StatsWindow::StatsWindow(QWidget *parent, LCSeqTools *P_Core) :
    QWidget(parent),
    ui(new Ui::StatsWindow){
    ui->setupUi(this);
    {
        int Index=ui->verticalLayout->indexOf(ui->tmpVerticalSpacer);
        ui->verticalLayout->removeItem(ui->verticalLayout->itemAt(Index));
        delete ui->tmpVerticalSpacer;
    }
    this->setWindowFlag(Qt::Dialog);
    Core=P_Core;
    try {
        Core->ReadLcvcftoolsStatsFile();
    } catch (QString msg) {
        QMessageBox::critical(this,"Error",msg,QMessageBox::Ok);
        return;
    }
    ChartView=new QChartView(this);
    ChartView->setRenderHint(QPainter::Antialiasing);
    ui->verticalLayout->insertWidget(0,ChartView);
    ChartView->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    ShowDepthPlot();
}
StatsWindow::~StatsWindow(){
    delete ui;
}
void StatsWindow::on_DepthPlotButton_clicked(){
    ShowDepthPlot();
}
void StatsWindow::on_QualityPlotButton_clicked(){
    ShowQualityPlot();
}
void StatsWindow::on_SubstitutionsButton_clicked(){
    ShowSubstitutionPlot();
}
void StatsWindow::SetChart(QChart *Chart){
    QChart* OldChart = ChartView->chart();
    ChartView->setChart(Chart);
    ui->IdentifyBrowser->setText("Hover the mouse over to get additional information.");
    ui->IdentifyBrowser->setAlignment(Qt::AlignCenter);
    ChartView->show();
    if(OldChart) OldChart->deleteLater();
}
double StatsWindow::FindMedian(const QVector<double> &tmpVector, int begin, int end){
    if(tmpVector.empty()) return 0;
    if(tmpVector.size()==1) return tmpVector.at(0);
    int count = end - begin;
    if (count % 2) {
        return tmpVector.at(count / 2 + begin);
    } else {
        qreal right = tmpVector.at(count / 2 + begin);
        qreal left = tmpVector.at(count / 2 - 1 + begin);
        return (right + left) / 2.0;
    }
}
void StatsWindow::Identify1(){
    QLineSeries* LineSeries = qobject_cast<QLineSeries*>(sender());
    const SampleStatsStruct& Sample = Core->SampleStats[LineSeries->name()];
    QString tmpString;
    tmpString += "ID=" + Sample.ID + "; ";
    tmpString += "Non-missing=" + QString::number(Sample.NMR*100,'g',3) + "%; ";
    tmpString += "Mean Depth=" + QString::number(Sample.MDP,'g',3) + "X; ";
    tmpString += "Mean Quality=" + QString::number(Sample.MGQ,'g',3) + "; ";
    ui->IdentifyBrowser->setText(tmpString);
    ui->IdentifyBrowser->setAlignment(Qt::AlignCenter);
}
void StatsWindow::Identify2(){
    QBarSeries* BarSeries = qobject_cast<QBarSeries*>(sender());
    QString Type=BarSeries->name();
    int TypeCount = SnpStats.SubstitutionTypes[Type];
    QString tmpString;
    tmpString += "Substitution=" + Type.replace(">"," to ") + "; ";
    if((Type.contains('A')&&Type.contains('G'))||(Type.contains('C')&&Type.contains('T')))
        tmpString += "Type=Transition; ";
    else
        tmpString += "Type=Transversion; ";
    tmpString += "Total=" + QString::number(TypeCount) + "; ";
    tmpString += "Percent=" + QString::number(static_cast<double>(TypeCount*100)/(SnpStats.Ts+SnpStats.Tv),'g',3) + "%; ";
    //ui->IdentifyBrowser->setTextColor(Qt::GlobalColor::black);
    ui->IdentifyBrowser->setText(tmpString);
    ui->IdentifyBrowser->setAlignment(Qt::AlignCenter);
}
void StatsWindow::ShowDepthPlot(){
    QChart *chart = new QChart();
    // Lineplot -------------------------------------------
    QMap<int,QVector<double>> tmpDepthMap;
    QList<QLineSeries*> LineSeriesList;
    double YLim(0);
    for(const auto& Sample : Core->SampleStats){
        QLineSeries *tmpLineSeries = new QLineSeries(chart);
        for(const int& Key: Sample.DP.keys()){
            //if(Key<0 || Key>XLim) continue;
            if(Key<0) continue;
            if(YLim<Sample.DP[Key]) YLim=Sample.DP[Key];
            tmpDepthMap[Key].append(Sample.DP[Key]);
            tmpLineSeries->append(Key,Sample.DP[Key]*100);
        }
        tmpLineSeries->setName(Sample.ID);
        connect(tmpLineSeries,&QLineSeries::hovered,this,&StatsWindow::Identify1);
        LineSeriesList.append(tmpLineSeries);
    }
    // Boxplot -------------------------------------------
    QBoxPlotSeries* BoxplotSeries = new QBoxPlotSeries(chart);
    {
        QBoxSet* tmpBoxSet = new QBoxSet("",chart);
        tmpBoxSet->setValue(QBoxSet::LowerExtreme,-1);
        tmpBoxSet->setValue(QBoxSet::LowerQuartile,-1);
        tmpBoxSet->setValue(QBoxSet::Median,-1);
        tmpBoxSet->setValue(QBoxSet::UpperQuartile,-1);
        tmpBoxSet->setValue(QBoxSet::UpperExtreme,-1);
        BoxplotSeries->append(tmpBoxSet);
    }
    int XLim=XLimThreshold_DepthMax;
    for(int Key: tmpDepthMap.keys()){
        auto& tmpVector=tmpDepthMap[Key];
        std::sort(tmpVector.begin(), tmpVector.end());
        QBoxSet* tmpBoxSet = new QBoxSet("",chart);
        int Count = tmpVector.size();
        double Median=FindMedian(tmpVector,0,Count);
        double Lower=FindMedian(tmpVector,0,Count/2);
        double Upper=FindMedian(tmpVector,Count/2+(Count%2),Count);
        double Min=tmpVector.first();
        double Max=tmpVector.last();
        tmpBoxSet->setValue(QBoxSet::LowerExtreme,Min*100);
        tmpBoxSet->setValue(QBoxSet::LowerQuartile,Lower*100);
        tmpBoxSet->setValue(QBoxSet::Median,Median*100);
        tmpBoxSet->setValue(QBoxSet::UpperQuartile,Upper*100);
        tmpBoxSet->setValue(QBoxSet::UpperExtreme,Max*100);
        if(Max<XLimThreshold_DepthPct && Key<XLim) XLim=Key;
        BoxplotSeries->append(tmpBoxSet);
    }
    BoxplotSeries->setBrush(QBrush(QColor(0,0,255,150)));
    // Chart -------------------------------------------
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->legend()->setVisible(false);
    chart->addSeries(BoxplotSeries);
    for(auto* LineSeries : LineSeriesList) {
        LineSeries->setOpacity(0.2);
        chart->addSeries(LineSeries);
    }
    // X Axis -------------------------------------------
    QValueAxis* AxisX;
    AxisX = new QValueAxis(chart);
    AxisX->setRange(0.001,XLim+0.5);
    AxisX->setTickType(QValueAxis::TicksDynamic);
    AxisX->setTickAnchor(1);
    AxisX->setTickInterval(1);
    AxisX->setLabelFormat("%dX");
    chart->addAxis(AxisX,Qt::AlignBottom);
    for(auto Series : chart->series()) Series->attachAxis(AxisX);
    // Y Axis -------------------------------------------
    QValueAxis* AxisY;
    AxisY = new QValueAxis(chart);
    AxisY->setRange(0,YLim*100);
    AxisY->setTickType(QValueAxis::TicksDynamic);
    AxisY->setTickAnchor(0);
    AxisY->setTickInterval(10);
    AxisY->setLabelFormat("%d%");
    chart->addAxis(AxisY,Qt::AlignLeft);
    for(auto Series : chart->series()) Series->attachAxis(AxisY);
    SetChart(chart);
}
void StatsWindow::ShowQualityPlot(){
    QChart *chart = new QChart();
    // Lineplot -------------------------------------------
    QMap<int,QVector<double>> tmpQualityMap;
    QList<QLineSeries*> LineSeriesList;
    double YLim(0);
    for(const auto& Sample : Core->SampleStats){
        QLineSeries *tmpLineSeries = new QLineSeries(chart);
        for(const int& Key: Sample.GQ.keys()){
            //if(Key<0 || Key>XLim) continue;
            if(Key<0) continue;
            if(YLim<Sample.GQ[Key]) YLim=Sample.GQ[Key];
            tmpQualityMap[Key].append(Sample.GQ[Key]);
            tmpLineSeries->append(Key,Sample.GQ[Key]*100);
        }
        tmpLineSeries->setName(Sample.ID);
        connect(tmpLineSeries,&QLineSeries::hovered,this,&StatsWindow::Identify1);
        LineSeriesList.append(tmpLineSeries);
    }
    // Boxplot -------------------------------------------
    QBoxPlotSeries* BoxplotSeries = new QBoxPlotSeries(this);
    {
        QBoxSet* tmpBoxSet = new QBoxSet("",chart);
        tmpBoxSet->setValue(QBoxSet::LowerExtreme,-1);
        tmpBoxSet->setValue(QBoxSet::LowerQuartile,-1);
        tmpBoxSet->setValue(QBoxSet::Median,-1);
        tmpBoxSet->setValue(QBoxSet::UpperQuartile,-1);
        tmpBoxSet->setValue(QBoxSet::UpperExtreme,-1);
        BoxplotSeries->append(tmpBoxSet);
    }
    int XLim=XLimThreshold_QualMax;
    for(int Key: tmpQualityMap.keys()){
        auto& tmpVector=tmpQualityMap[Key];
        std::sort(tmpVector.begin(), tmpVector.end());
        QBoxSet* tmpBoxSet = new QBoxSet("",chart);
        int Count = tmpVector.size();
        double Median=FindMedian(tmpVector,0,Count);
        double Lower=FindMedian(tmpVector,0,Count/2);
        double Upper=FindMedian(tmpVector,Count/2+(Count%2),Count);
        double Min=tmpVector.first();
        double Max=tmpVector.last();
        tmpBoxSet->setValue(QBoxSet::LowerExtreme,Min*100);
        tmpBoxSet->setValue(QBoxSet::LowerQuartile,Lower*100);
        tmpBoxSet->setValue(QBoxSet::Median,Median*100);
        tmpBoxSet->setValue(QBoxSet::UpperQuartile,Upper*100);
        tmpBoxSet->setValue(QBoxSet::UpperExtreme,Max*100);
        if(Max<XLimThreshold_QualPct && Key<XLim) XLim=Key;
        BoxplotSeries->append(tmpBoxSet);
    }
    BoxplotSeries->setBrush(QBrush(QColor(255,0,0,150)));
    // Chart -------------------------------------------
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->legend()->setVisible(false);
    chart->addSeries(BoxplotSeries);
    for(auto* LineSeries : LineSeriesList) {
        LineSeries->setOpacity(0.2);
        chart->addSeries(LineSeries);
    }
    // X Axis -------------------------------------------
    QValueAxis* AxisX;
    AxisX = new QValueAxis(chart);
    AxisX->setRange(0.001,XLim+0.5);
    AxisX->setTickType(QValueAxis::TicksDynamic);
    AxisX->setTickAnchor(0);
    AxisX->setTickInterval(10);
    AxisX->setLabelFormat("%d");
    chart->addAxis(AxisX,Qt::AlignBottom);
    for(auto Series : chart->series()) Series->attachAxis(AxisX);
    // Y Axis -------------------------------------------
    QValueAxis* AxisY;
    AxisY = new QValueAxis(chart);
    AxisY->setRange(0,YLim*100);
    AxisY->setTickType(QValueAxis::TicksDynamic);
    AxisY->setTickAnchor(0);
    AxisY->setTickInterval(10);
    AxisY->setLabelFormat("%d%");
    chart->addAxis(AxisY,Qt::AlignLeft);
    for(auto Series : chart->series()) Series->attachAxis(AxisY);
    SetChart(chart);
}
void StatsWindow::ShowSubstitutionPlot(){
    QChart *chart = new QChart();
    SnpStats = Core->ReadBcftoolsStatsFile();
    QList<QBarSeries*> TsList, TvList;
    for(QString Key : SnpStats.SubstitutionTypes.keys()){
        QBarSet* tmpBarSet = new QBarSet(Key,chart);
        double tmpValue = static_cast<double>(SnpStats.SubstitutionTypes[Key]*100)/(SnpStats.Ts+SnpStats.Tv);
        tmpBarSet->append(tmpValue);
        QBarSeries* tmpBarSeries = new QBarSeries(chart);
        tmpBarSeries->append(tmpBarSet);
        tmpBarSeries->setName(Key);
        connect(tmpBarSeries,&QBarSeries::hovered,this,&StatsWindow::Identify2);
        if((Key.contains('A')&&Key.contains('G'))||(Key.contains('C')&&Key.contains('T'))){
            tmpBarSet->setLabel(Key.replace(">","<sup>TS</sup>"));
            TsList.append(tmpBarSeries);
        } else {
            tmpBarSet->setLabel(Key.replace(">","<sup>TV</sup>"));
            TvList.append(tmpBarSeries);
        }
    }
    // Add Series
    for(auto Series : TsList) chart->addSeries(Series);
    for(auto Series : TvList) chart->addSeries(Series);
    // chart & series -------------------------------------------
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
    chart->setAnimationOptions(QChart::SeriesAnimations);
    // Y Axis -------------------------------------------
    QValueAxis* AxisY;
    AxisY = new QValueAxis(chart);
    AxisY->setTickType(QValueAxis::TicksDynamic);
    AxisY->setTickAnchor(0);
    AxisY->setTickInterval(1);
    AxisY->setLabelFormat("%d%");
    chart->addAxis(AxisY,Qt::AlignLeft);
    for(auto Series : chart->series()) Series->attachAxis(AxisY);
    SetChart(chart);
}
