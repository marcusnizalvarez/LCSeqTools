#ifndef REFERENCEEDITOR_H
#define REFERENCEEDITOR_H

#include "lcseqtools_cli.h"
#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QMovie>
#include <QtConcurrent/QtConcurrent>
namespace Ui {
class ReferenceEditor;
}
class ReferenceEditor : public QDialog
{
    Q_OBJECT

public:
    explicit ReferenceEditor(QWidget *parent, LCSeqTools *P_Core);
    ~ReferenceEditor();
private slots:
    void on_tableWidget_itemSelectionChanged();
    void on_CloseButton_clicked();
    void on_DiploidButton_clicked();
    void on_HaploidButton_clicked();
    void on_IgnoredButton_clicked();
    void on_ImportDiploidButton_clicked();
    void on_ImportHaploidButton_clicked();
    void on_ImportIgnoredButton_clicked();
    void UpdateTable();
signals:
    void UpdateTableSignal();
private:
    void SetPloidy(int Ploidy);
    QStringList ImportList();
    void GetSequenceIds();
    void GetSequenceIdsFailed(QString Message);
    Ui::ReferenceEditor *ui;
    LCSeqTools *Core;
    enum {
        ReferenceEditor_Loading=0,
        ReferenceEditor_Editing=1
    } ReferenceEditorIndex;
};

#endif // REFERENCEEDITOR_H
