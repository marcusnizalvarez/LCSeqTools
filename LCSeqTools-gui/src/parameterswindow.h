#ifndef PARAMETERSWINDOW_H
#define PARAMETERSWINDOW_H

#include <QThread>
#include <QWidget>
#include "lcseqtools_cli.h"

namespace Ui {
class ParametersWindow;
}

class ParametersWindow : public QWidget
{
    Q_OBJECT
signals:
    void WidgetClosed();
public:
    explicit ParametersWindow(QWidget *parent, LCSeqTools *P_Core);
    ~ParametersWindow();
private slots:
    void on_DefaultButton_clicked();
    void on_trimmomatic_trailing_crop_valueChanged(int arg1);
    void on_trimmomatic_head_crop_valueChanged(int arg1);
    void on_trimmomatic_min_len_valueChanged(int arg1);
    void on_lcvcftools_minGQ_valueChanged(int arg1);
    void on_lcvcftools_minDP_valueChanged(int arg1);
    void on_lcvcftools_maf_valueChanged(double arg1);
    void on_lcvcftools_max_missing_sample_valueChanged(double arg1);
    void on_lcvcftools_max_missing_variant_valueChanged(double arg1);
    void on_beagle_isLowMemoryEnabled_stateChanged(int arg1);
    void on_beagle_ImputationSeed_valueChanged(int arg1);
    void on_bcftools_minGP_valueChanged(double arg1);
    void on_OKButton_clicked();
    void on_global_maxthreads_valueChanged(int arg1);
    void on_beagle_isImputEnable_stateChanged(int arg1);

private:
    void UpdateThis();
    Ui::ParametersWindow *ui;
    void closeEvent(QCloseEvent *event);
    Project* MyProject;
};

#endif // PARAMETERSWINDOW_H
