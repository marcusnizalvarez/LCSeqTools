#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QApplication>
#include <QDesktopServices>
#include <QtConcurrent/QtConcurrent>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QGraphicsOpacityEffect>
#include <QInputDialog>
#include <QMainWindow>
#include <QMessageBox>
#include <QMovie>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QRegularExpression>
#include <QScreen>
#include <QSplashScreen>
#include <QTableWidget>
#include <QThread>
#include "statswindow.h"
#include "referenceeditor.h"
#include "samplemanager.h"
#include "fastqcviewer.h"
#include "parameterswindow.h"
#include "lcseqtools_cli.h"
#define MIN_N 8
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    void Splashscreen();
    void TestMode();
    ~MainWindow();
private slots:
    void ChangeCurrentPage(int CurrentIndex = 0);
    void FailMsgFromCoreApp(QString ErrorMsg);
    void on_AddSampleButton_clicked();
    void on_EditSampleButton_clicked();
    void on_RemoveSampleButton_clicked();
    void on_SetReferenceButton_clicked();
    void on_SetParametersButton_clicked();
    void on_SamplesTableWidget_itemSelectionChanged();
    void on_NewProjectButton_triggered();
    void on_actionExit_triggered();
    void on_actionOpen_Project_Statistics_triggered();
    void on_actionAbout_triggered();
    void on_actionRun_FastQC_triggered();
    void on_actionSet_Reference_Ploidy_triggered();
    void on_actionMark_selected_as_Resequecing_triggered();
    void on_actionUnmark_selected_as_Resequecing_triggered();
    void on_actionMark_Selected_As_Ignored_Illumina_Pattern_triggered();
    void on_actionUnmark_Selected_As_Ignored_Illumina_Pattern_triggered();
    void on_StartButton_clicked();
    void on_SetProjectPathButton_clicked();
    void page2_UpdateStatus(int WhichStep, int Status);
    void on_page2_CheckStatisticsButton_clicked();
    void on_page2_GoBackButton_clicked();
    void on_page2_ExitButton_clicked();
private:
    void on_AddSample_autodetect();
    void on_AddSample_importCSV();
    void on_AddSample_manual();
    void page1_UpdateTable();
    void page1_UpdateReference();
    void page1_UpdateProjectPath();
    void page1_UpdateParameters();
    void page1_UpdateStartButton();
    bool page1_isConfiguredTable = false;
    bool page1_isConfiguredReference = false;
    bool page1_isConfiguredProjectPath = false;
    bool page1_isConfiguredParameters = false;
    void page2_AllStepsFinished();
    void RunCoreApplication();
    Ui::MainWindow *ui;
    LCSeqTools CoreApplication;
    QFuture<void> Future;
    typedef enum {
        MainWindow_WelcomePage=0,
        MainWindow_EditingPage=1,
        MainWindow_RunningPage=2
    } AppCurrentStatus;
};
#endif // MAINWINDOW_H
