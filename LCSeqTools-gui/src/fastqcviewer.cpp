#include "fastqcviewer.h"
#include "ui_fastqcviewer.h"

FastqcViewer::FastqcViewer(QWidget *parent, QStringList Files) :
    QDialog(parent),
    ui(new Ui::FastqcViewer){
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(FastqcViewer_Loading);
    ui->LoadingIcon->clear();
    ui->LoadingIcon->setMovie(new QMovie(":/etc/loader.gif"));
    ui->LoadingIcon->movie()->start();
    connect(this,&FastqcViewer::UpdateHtmlSignal,this,&FastqcViewer::UpdateHtml);
    QtConcurrent::run(this,&FastqcViewer::ReadFastqc,Files);
}

FastqcViewer::~FastqcViewer(){
    delete ui;
}

void FastqcViewer::ReadFastqc(QStringList Files){
    try {
        QFile ScriptFile(":/bash/fastqc.sh");
        QTemporaryFile *tmpScriptFile = QTemporaryFile::createNativeFile(ScriptFile);
        QStringList Arguments;
        Arguments.append(tmpScriptFile->fileName());
        Arguments.append(Files);
        QProcess SystemCall;
        connect(this,&FastqcViewer::destroyed,&SystemCall,&QProcess::kill);
        SystemCall.setProcessChannelMode(QProcess::SeparateChannels);
        SystemCall.start("/bin/bash",Arguments);
        SystemCall.waitForFinished(-1);
        tmpScriptFile->remove();
        QFile File ("/tmp/sequences_fastqc.html");
        File.open(QIODevice::ReadOnly);
        QTextStream Stream(&File);
        while(!Stream.atEnd()){
            QString Line=Stream.readLine();
            HtmlBody.append(Line);
        }
    } catch (int) {}
    emit UpdateHtmlSignal();
}

void FastqcViewer::UpdateHtml(){
    ui->textBrowser->setHtml(HtmlBody);
    ui->stackedWidget->setCurrentIndex(FastqcViewer_Showing);
}
