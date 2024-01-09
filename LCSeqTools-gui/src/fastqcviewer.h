#ifndef FASTQCVIEWER_H
#define FASTQCVIEWER_H

#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QMovie>
#include <QtConcurrent/QtConcurrent>

namespace Ui {
class FastqcViewer;
}

class FastqcViewer : public QDialog
{
    Q_OBJECT

public:
    explicit FastqcViewer(QWidget *parent, QStringList Files);
    void ReadFastqc(QStringList Files);
    void ShowFastqc();
    ~FastqcViewer();
private slots:
    void UpdateHtml();
signals:
    void UpdateHtmlSignal();
private:
    Ui::FastqcViewer *ui;
    QString HtmlBody;
    enum {
        FastqcViewer_Loading=0,
        FastqcViewer_Showing=1
    } FastqcViewerIndex;
};

#endif // FASTQCVIEWER_H
