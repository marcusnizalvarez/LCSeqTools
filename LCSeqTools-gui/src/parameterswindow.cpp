#include "parameterswindow.h"
#include "ui_parameterswindow.h"

ParametersWindow::ParametersWindow(QWidget *parent, LCSeqTools *P_Core) :
    QWidget(parent),
    ui(new Ui::ParametersWindow){
    ui->setupUi(this);
    this->setWindowFlag(Qt::Dialog);
    ui->global_maxthreads->setMaximum(QThread::idealThreadCount());
    MyProject = &P_Core->MyProject;
    UpdateThis();
}

ParametersWindow::~ParametersWindow(){
    delete ui;
}

void ParametersWindow::closeEvent(QCloseEvent *event){
    if(event) emit WidgetClosed();
}

void ParametersWindow::on_DefaultButton_clicked(){
    ParametersStruct tmpDefaultParameters;
    MyProject->Threads=QThread::idealThreadCount();
    MyProject->Parameters = tmpDefaultParameters;
    UpdateThis();
}

void ParametersWindow::UpdateThis(){
    ui->global_maxthreads->setValue(MyProject->Threads);
    ui->trimmomatic_head_crop->setValue(MyProject->Parameters.trimmomatic_head_crop);
    ui->trimmomatic_trailing_crop->setValue(MyProject->Parameters.trimmomatic_trailing_crop);
    ui->trimmomatic_min_len->setValue(MyProject->Parameters.trimmomatic_min_len);
    ui->lcvcftools_minGQ->setValue(MyProject->Parameters.lcvcftools_minGQ);
    ui->lcvcftools_minDP->setValue(MyProject->Parameters.lcvcftools_minDP);
    ui->lcvcftools_maf->setValue(MyProject->Parameters.lcvcftools_maf);
    ui->lcvcftools_max_missing_sample->setValue(MyProject->Parameters.other_max_missing_sample);
    ui->lcvcftools_max_missing_variant->setValue(MyProject->Parameters.lcvcftools_max_missing_variant);
    ui->beagle_isImputEnable->setChecked(MyProject->isImputEnable);
    ui->beagle_ImputationSeed->setValue(MyProject->Parameters.beagle_ImputationSeed);
    ui->beagle_isLowMemoryEnabled->setChecked(MyProject->Parameters.beagle_isLowMemoryEnabled);
    ui->bcftools_minGP->setValue(MyProject->Parameters.bcftools_minGP);
}

void ParametersWindow::on_global_maxthreads_valueChanged(int arg1){
    MyProject->Threads=arg1;
}

void ParametersWindow::on_trimmomatic_trailing_crop_valueChanged(int arg1){
    MyProject->Parameters.trimmomatic_trailing_crop=arg1;
}

void ParametersWindow::on_trimmomatic_head_crop_valueChanged(int arg1){
    MyProject->Parameters.trimmomatic_head_crop=arg1;
}

void ParametersWindow::on_trimmomatic_min_len_valueChanged(int arg1){
    MyProject->Parameters.trimmomatic_min_len=arg1;
}

void ParametersWindow::on_lcvcftools_minGQ_valueChanged(int arg1){
    MyProject->Parameters.lcvcftools_minGQ=arg1;
}

void ParametersWindow::on_lcvcftools_minDP_valueChanged(int arg1){
    MyProject->Parameters.lcvcftools_minDP=arg1;
}

void ParametersWindow::on_lcvcftools_maf_valueChanged(double arg1){
    MyProject->Parameters.lcvcftools_maf=arg1;
}

void ParametersWindow::on_lcvcftools_max_missing_sample_valueChanged(double arg1){
    MyProject->Parameters.other_max_missing_sample=arg1;
}

void ParametersWindow::on_lcvcftools_max_missing_variant_valueChanged(double arg1){
    MyProject->Parameters.lcvcftools_max_missing_variant=arg1;
}

void ParametersWindow::on_beagle_isLowMemoryEnabled_stateChanged(int arg1){
    MyProject->Parameters.beagle_isLowMemoryEnabled=arg1;
}

void ParametersWindow::on_beagle_ImputationSeed_valueChanged(int arg1){
    MyProject->Parameters.beagle_ImputationSeed=arg1;
}

void ParametersWindow::on_bcftools_minGP_valueChanged(double arg1){
    MyProject->Parameters.bcftools_minGP=arg1;
}

void ParametersWindow::on_OKButton_clicked(){
    MyProject->isConfigured=true;
    this->close();
}

void ParametersWindow::on_beagle_isImputEnable_stateChanged(int arg1){
    if(MyProject->isImputEnable != bool(arg1) )
        MyProject->isImputEnable=arg1;
    ui->beagle_ImputationSeed->setEnabled(arg1);
    ui->beagle_isLowMemoryEnabled->setEnabled(arg1);
    ui->bcftools_minGP->setEnabled(arg1);
}
