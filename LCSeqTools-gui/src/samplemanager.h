#ifndef SAMPLEMANAGER_H
#define SAMPLEMANAGER_H

#include <QWidget>
#include <QStringListModel>
#include <QFileDialog>
#include "lcseqtools_cli.h"

namespace Ui {
class SampleManager;
}

class SampleManager : public QWidget
{
    Q_OBJECT
public:
    explicit SampleManager(QWidget *parent, SampleStruct* P_Sample, LCSeqTools *P_Core);
    ~SampleManager();
signals:
    void WidgetClosed();
private slots:
    // Buttons
    void on_ResetNameButton_clicked();
    void on_SetFileButton_clicked();
    void on_RemoveFileButton_clicked();
    void on_OKButton_clicked();
    void on_AddLaneButton_clicked();
    void on_RemoveLaneButton_clicked();

    // Events
    void on_SampleNameLineEdit_textChanged(const QString &arg1);
    void on_FilesTableWidget_itemSelectionChanged();

    void on_IgnoreUnmatchingFilenameBox_stateChanged(int arg1);

    void on_IsReseqBox_stateChanged(int arg1);

private:
    void UpdateThis();
    int GetLaneCount();
    int GetFileCount();
    int LaneCount = 0;
    QStringList LaneLabels;
    QStringList MateLabels = {"R1","R2"};
    void closeEvent(QCloseEvent *event);
    QDir LastPath=QDir::homePath();
    QString ResetName;
    Ui::SampleManager *ui;
    SampleStruct *Sample;
    LCSeqTools *Core;
};

#endif // SAMPLEMANAGER_H
