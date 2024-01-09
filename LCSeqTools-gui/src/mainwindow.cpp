#include "mainwindow.h"
#include "ui_mainwindow.h"
//DebugMsg(__PRETTY_FUNCTION__);
MainWindow::~MainWindow(){
    delete ui;
}
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow){
    ui->setupUi(this);
    move(QGuiApplication::screens().at(0)->geometry().center() - frameGeometry().center());
    connect(&CoreApplication,&LCSeqTools::UpdateStatus,this,&MainWindow::page2_UpdateStatus);
    connect(&CoreApplication, &LCSeqTools::LogMsgSignal, ui->LogBrowser, &QTextBrowser::append);
    connect(&CoreApplication, &LCSeqTools::SystemCallSignal, ui->SystemCallBrowser, &QTextBrowser::append);
    CoreApplication.setParent(this);
}
void MainWindow::Splashscreen(){
    if(LCSeqTools::isDebug){
        ChangeCurrentPage(MainWindow_WelcomePage);
        this->show();
        return;
    }
    QSplashScreen* SplashScreen = new QSplashScreen(QPixmap("://etc/splash.png"));
    QGraphicsOpacityEffect *OpacityEff = new QGraphicsOpacityEffect(SplashScreen);
    QPropertyAnimation *Anim = new QPropertyAnimation(OpacityEff,"opacity");
    SplashScreen->setAttribute(Qt::WA_TranslucentBackground,true);
    SplashScreen->setAttribute(Qt::WA_NoBackground,true);
    SplashScreen->setAttribute(Qt::WA_NoSystemBackground,true);
    SplashScreen->setGraphicsEffect(OpacityEff);
    Anim->setDuration(1500);
    Anim->setStartValue(0);
    Anim->setEndValue(1);
    Anim->setEasingCurve(QEasingCurve::Linear);
    connect(Anim, &QPropertyAnimation::finished,[=](){
        QThread::msleep(1500);
        SplashScreen->close();
        ChangeCurrentPage(MainWindow_WelcomePage);
        this->show();
    });
    SplashScreen->show();
    Anim->start(QPropertyAnimation::DeleteWhenStopped);
}
void MainWindow::on_NewProjectButton_triggered(){
    ChangeCurrentPage(MainWindow_EditingPage);
    page1_UpdateTable();
    page1_UpdateReference();
    page1_UpdateParameters();
    page1_UpdateProjectPath();
}
void MainWindow::on_actionAbout_triggered(){
    QMessageBox msgBox(this);
    msgBox.setTextFormat(Qt::RichText);
    msgBox.setText("A user-friendly tool for variant calling and imputation using low-coverage whole genome sequencing data.<br><br><br>"
                   "Author: Marcus Vinicius Niz Alvarez<br>"
                   "E-mail: <a href='marcus.alvarez@unesp.br'>marcus.alvarez@unesp.br</a>");
    msgBox.exec();
}
void MainWindow::on_actionExit_triggered(){
    QApplication::exit();
}
void MainWindow::on_SamplesTableWidget_itemSelectionChanged(){
    bool tmpBool = ui->SamplesTableWidget->selectedItems().size()>0;
    ui->EditSampleButton->setEnabled(tmpBool);
    ui->RemoveSampleButton->setEnabled(tmpBool);
    ui->actionMark_selected_as_Resequecing->setEnabled(tmpBool);
    ui->actionUnmark_selected_as_Resequecing->setEnabled(tmpBool);
    ui->actionMark_Selected_As_Ignored_Illumina_Pattern->setEnabled(tmpBool);
    ui->actionUnmark_Selected_As_Ignored_Illumina_Pattern->setEnabled(tmpBool);
    ui->actionRun_FastQC->setEnabled(tmpBool);
}
void MainWindow::on_actionRun_FastQC_triggered(){
    QItemSelectionModel *Selected = ui->SamplesTableWidget->selectionModel();
    if(!Selected->hasSelection()) return;
    QStringList Files;
    for(auto i : Selected->selectedRows()) {
        QList<QStringList> FileLists = CoreApplication.GetFileLists(CoreApplication.MyProject.SampleList[i.row()]);
        for(QStringList FileList : FileLists)
            for(QString File : FileList)
                Files.append(File.remove(';'));
    }
    FastqcViewer* fastqcViewer = new FastqcViewer(this, Files);
    fastqcViewer->show();
}
void MainWindow::on_AddSampleButton_clicked(){
    QWidget *Widget = new QWidget(this);
    Widget->setWindowFlag(Qt::Dialog);
    Widget->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    QVBoxLayout *Box = new QVBoxLayout(Widget);
    QPushButton *Button;
    // Autodetec
    Button = new QPushButton(Widget);
    Button->setMinimumSize(150,30);
    Button->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    Button->setText("Import Folder");
    Box->addWidget(Button);
    connect(Button,&QPushButton::clicked,this,[=](){this->on_AddSample_autodetect();});
    connect(Button,&QPushButton::clicked,Widget,&QWidget::close);
    // Import CSV
    Button = new QPushButton(Widget);
    Button->setMinimumSize(150,30);
    Button->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    Button->setText("Import Sample-sheet");
    Box->addWidget(Button);
    connect(Button,&QPushButton::clicked,this,[=](){this->on_AddSample_importCSV();});
    connect(Button,&QPushButton::clicked,Widget,&QWidget::close);
    // Manual
    Button = new QPushButton(Widget);
    Button->setMinimumSize(150,30);
    Button->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    Button->setText("Add Manually");
    Box->addWidget(Button);
    connect(Button,&QPushButton::clicked,this,[=](){this->on_AddSample_manual();});
    connect(Button,&QPushButton::clicked,Widget,&QWidget::close);
    // Show
    Widget->setLayout(Box);
    Widget->show();
    return;
}
void MainWindow::on_actionOpen_Project_Statistics_triggered(){
    QString DirName = QFileDialog::getExistingDirectory(this,
                                                        ("Select an existing project folder"),
                                                        QDir::homePath());
    if(DirName.isEmpty()) return;
    CoreApplication.MyProject.ProjectPath=DirName;
    StatsWindow *Stats = new StatsWindow(this,&CoreApplication);
    if(Stats->IsValid()){
        Stats->show();
    }
    else{
        Stats->close();
    }
    return;
}
void MainWindow::on_EditSampleButton_clicked(){
    QItemSelectionModel *Selected = ui->SamplesTableWidget->selectionModel();
    if(!Selected->hasSelection()) return;
    if(Selected->selectedRows().size()>1) {
        QMessageBox::information(this,"Information","You must select one row at time");
        return;
    }
    SampleManager *manager = new SampleManager(this,
            &CoreApplication.MyProject.SampleList[ui->SamplesTableWidget->selectedItems().first()->row()],
            &CoreApplication);
    connect(manager, &SampleManager::WidgetClosed, this, &MainWindow::page1_UpdateTable);
    manager->show();
}
void MainWindow::on_RemoveSampleButton_clicked(){
    QItemSelectionModel *Selected = ui->SamplesTableWidget->selectionModel();
    if(!Selected->hasSelection()) return;
    QMessageBox::StandardButton reply = QMessageBox::question(this,
                                  "Remove","Are you sure?\n",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        auto SelectedRows=Selected->selectedRows();
        std::sort(SelectedRows.begin(),SelectedRows.end());
        for(int i(0); i < SelectedRows.size(); i++) CoreApplication.MyProject.SampleList.removeAt(SelectedRows[i].row()-i);
        page1_UpdateTable();
    }
}
void MainWindow::on_SetProjectPathButton_clicked(){
    QString DirName = QFileDialog::getExistingDirectory(this,
                                                        ("Select a folder"),
                                                        QDir::homePath());
    if(DirName.isEmpty()) return;
    CoreApplication.MyProject.ProjectPath=DirName;
    if(!CoreApplication.CheckIsValidProjectPath()){
        QMessageBox::StandardButton reply = QMessageBox::question(this,
                                      "Warning","Project path is not empty. Do you want to delete existing files?\n",
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            if(!CoreApplication.DeleteExistingProject())
                QMessageBox::critical(this,"Error","Failed to delete files.",QMessageBox::Ok);
        }
    }
    page1_UpdateProjectPath();
}
void MainWindow::on_SetReferenceButton_clicked(){
    QString File = QFileDialog::getOpenFileName(this,
                                                ("Select a file"),
                                                QDir::homePath(),
                                                "FASTA file (*.fa *.fasta *.fna *.fa.gz *.fasta.gz *.fna.gz)");
    if(File.isEmpty()) return;
    CoreApplication.MyProject.FastaReferenceFile=File;
    page1_UpdateReference();
}
void MainWindow::on_AddSample_autodetect(){
    QString ParentFolder = QFileDialog::getExistingDirectory(this,
                                                             ("Select a folder"),
                                                             QDir::homePath());
    if(ParentFolder.isEmpty()) return;
    QDirIterator it(ParentFolder,
                    QStringList() << "*.fq.gz" << "*.fastq.gz",
                    QDir::NoFilter,
                    QDirIterator::Subdirectories);
    QStringList FileList;
    while(it.hasNext())
        FileList.push_back(it.next());
    try {
        if(FileList.isEmpty())
            throw QString ("No FASTQ files detected!");
        QList<SampleStruct> TmpSampleList;
        for (QString File : FileList) {
            if(File.count('/') - ParentFolder.count('/') > 3)
                throw QString("Too many subfolders at ") + ParentFolder;
            QStringList Match(CoreApplication.GetIlluminaRegex(File));
            if(Match.empty())
                throw QString("File(s) unmatching Illumina pattern");
            QString SampleID = Match.at(0);
            QString SampleFID = Match.at(1);
            QString SampleLane = Match.at(2);
            QString SampleMate = Match.at(3);
            bool isNew(true), isNoEmpty(true);
            for(auto& TmpSample : TmpSampleList)
                if(TmpSample.ID==SampleID && TmpSample.FID==SampleFID){
                    isNew=false;
                    if(TmpSample.FastqFiles[SampleLane][SampleMate].isEmpty()){
                        TmpSample.FastqFiles[SampleLane][SampleMate]=File;
                        isNoEmpty=false;
                        break;
                    }
                }
            if(isNew){
                SampleStruct TmpSample;
                TmpSample.ID = SampleID;
                TmpSample.FID = SampleFID;
                TmpSample.FastqFiles[SampleLane][SampleMate]=File;
                TmpSampleList.push_back(TmpSample);
            }
            else if(isNoEmpty){
                SampleStruct TmpSample;
                TmpSample.ID = SampleID;
                TmpSample.FID = SampleFID;
                TmpSample.FastqFiles[SampleLane][SampleMate]=File;
                TmpSampleList.push_back(TmpSample);
            }
        }
        CoreApplication.MyProject.SampleList.append(TmpSampleList);
        page1_UpdateTable();
    } catch (QString msg) {
        QMessageBox::critical(this,"Error",msg,QMessageBox::Ok);
        return;
    }
}
void MainWindow::on_AddSample_importCSV(){
    QString Filename = QFileDialog::getOpenFileName(this,
                                                    "Select a CSV sample-sheet file",
                                                    QDir::homePath(),
                                                    tr("Table (*.csv)"));
    if(Filename.isEmpty()) return;
    try {
        QMessageBox::StandardButton reply = QMessageBox::question(this,
                                      "Question","Do you want to ignore if filename Mate/Lane does not match with sample-sheet header?\n",
                                      QMessageBox::Yes|QMessageBox::No);
        CoreApplication.MyProject.isIgnoreUnmatchingFilenames=reply==QMessageBox::Yes;
        CoreApplication.ReadSampleSheet(Filename);
    } catch (QString msg) {
        QMessageBox::warning(this,"Error",msg);
    }
    page1_UpdateTable();
}
void MainWindow::on_AddSample_manual(){
    SampleStruct newSample;
    newSample.ID="unnamed";
    CoreApplication.MyProject.SampleList.push_back(std::move(newSample));
    SampleManager *manager = new SampleManager(this,
                                               &CoreApplication.MyProject.SampleList.last(),
                                               &CoreApplication);
    manager->setWindowFlags(Qt::Dialog);
    connect(manager, &SampleManager::WidgetClosed, this, &MainWindow::page1_UpdateTable);
    manager->show();
}
void MainWindow::on_SetParametersButton_clicked(){
    ParametersWindow* parameters = new ParametersWindow(this,&CoreApplication);
    connect(parameters, &ParametersWindow::WidgetClosed, this, &MainWindow::page1_UpdateParameters);
    parameters->show();
}
void MainWindow::on_actionSet_Reference_Ploidy_triggered(){
    ReferenceEditor* referenceEditor = new ReferenceEditor(this, &CoreApplication);
    referenceEditor->show();
}
void MainWindow::on_actionMark_selected_as_Resequecing_triggered(){
    QItemSelectionModel *Selected = ui->SamplesTableWidget->selectionModel();
    if(!Selected->hasSelection()) return;
    for(auto i : Selected->selectedRows()) CoreApplication.MyProject.SampleList[i.row()].isResequencing=true;
    page1_UpdateTable();
};
void MainWindow::on_actionUnmark_selected_as_Resequecing_triggered(){
    QItemSelectionModel *Selected = ui->SamplesTableWidget->selectionModel();
    if(!Selected->hasSelection()) return;
    for(auto i : Selected->selectedRows()) CoreApplication.MyProject.SampleList[i.row()].isResequencing=false;
    page1_UpdateTable();
    page1_UpdateTable();
};
void MainWindow::on_actionMark_Selected_As_Ignored_Illumina_Pattern_triggered(){
    QItemSelectionModel *Selected = ui->SamplesTableWidget->selectionModel();
    if(!Selected->hasSelection()) return;
    for(auto i : Selected->selectedRows()) CoreApplication.MyProject.SampleList[i.row()].isIgnoreUnmatchingFilename=true;
    page1_UpdateTable();
}
void MainWindow::on_actionUnmark_Selected_As_Ignored_Illumina_Pattern_triggered(){
    QItemSelectionModel *Selected = ui->SamplesTableWidget->selectionModel();
    if(!Selected->hasSelection()) return;
    for(auto i : Selected->selectedRows()) CoreApplication.MyProject.SampleList[i.row()].isIgnoreUnmatchingFilename=false;
    page1_UpdateTable();
}
void MainWindow::page1_UpdateReference(){
    if(CoreApplication.MyProject.FastaReferenceFile.isEmpty()){
        ui->actionSet_Reference_Ploidy->setEnabled(false);
        ui->ReferenceCheckIcon->setPixmap(QPixmap(":/etc/missing.png"));
        ui->ReferenceFileLabel->setText("Unconfigured.");
        ui->ReferenceFileLabel->setToolTip("Unconfigured.");
        page1_isConfiguredReference=false;
        return;
    }
    bool isConfig = CoreApplication.CheckIsExistingRefFile();
    ui->ReferenceCheckIcon->setPixmap(QPixmap(isConfig?":/etc/checked.png":":/etc/error.png"));
    ui->ReferenceFileLabel->setText("Reference file: " + QFileInfo(CoreApplication.MyProject.FastaReferenceFile).fileName());
    ui->ReferenceFileLabel->setToolTip(CoreApplication.MyProject.FastaReferenceFile);
    ui->actionSet_Reference_Ploidy->setEnabled(isConfig);
    page1_isConfiguredReference=isConfig;
    page1_UpdateStartButton();
    return;
}
void MainWindow::page1_UpdateParameters(){
    if(!CoreApplication.MyProject.isConfigured){
        ui->ParametersCheckIcon->setPixmap(QPixmap(":/etc/missing.png"));
        ui->ParametersLabel->setText("Unconfigured.");
        page1_isConfiguredParameters=false;
        return;
    }
    CoreApplication.ConvertParametersToFlagList();
    bool isConfig = CoreApplication.CheckIsParamConfigured();
    ui->ParametersCheckIcon->setPixmap(QPixmap(isConfig?":/etc/checked.png":":/etc/error.png"));
    ui->ParametersLabel->setText(isConfig?"Parameters configured.":"Invalid Parameters.");
    page1_isConfiguredParameters=isConfig;
    page1_UpdateStartButton();
    return;
}
void MainWindow::page1_UpdateProjectPath(){
    if(CoreApplication.MyProject.ProjectPath.isEmpty()){
        ui->ProjectPathCheckIcon->setPixmap(QPixmap(":/etc/missing.png"));
        ui->ProjectPathLabel->setText("Unconfigured.");
        page1_isConfiguredProjectPath=false;
        return;
    }
    bool isConfig = CoreApplication.CheckIsValidProjectPath();
    ui->ProjectPathCheckIcon->setPixmap(QPixmap(isConfig?":/etc/checked.png":":/etc/error.png"));
    ui->ProjectPathLabel->setText(CoreApplication.MyProject.ProjectPath);
    page1_isConfiguredProjectPath=isConfig;
    page1_UpdateStartButton();
    return;
}
void MainWindow::page1_UpdateTable(){
    std::sort(CoreApplication.MyProject.SampleList.begin(),CoreApplication.MyProject.SampleList.end(),[](const SampleStruct& a, const SampleStruct& b){return a.ID < b.ID;});
    bool isConfig = true;
    QTableWidget *table = ui->SamplesTableWidget;
    table->clear();
    // Rows
    table->setRowCount(CoreApplication.MyProject.SampleList.size());
    table->verticalHeader()->setVisible(false);
    // Columns
    QStringList HorizontalLabels = QStringList {"Name","Single Files","Paired Files","Status"};
    table->setColumnCount(HorizontalLabels.size());
    table->setHorizontalHeaderLabels(HorizontalLabels);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->horizontalHeader()->setVisible(true);
    // Build Table
    for(int i(0); i < CoreApplication.MyProject.SampleList.size(); i++){
        QTableWidgetItem *Item;
        const SampleStruct& Sample = CoreApplication.MyProject.SampleList[i];
        QList<QStringList> FileLists = CoreApplication.GetFileLists(Sample);

        // ID
        Item = new QTableWidgetItem;
        Item->setText(Sample.ID);
        if(Sample.isResequencing) {
            QFont tmpFont = this->font();
            tmpFont.setUnderline(true);
            Item->setFont(tmpFont);
        }
        Item->setTextAlignment(Qt::AlignCenter);
        table->setItem(i, 0, Item);

        // Single
        Item = new QTableWidgetItem;
        Item->setText(QString::number(FileLists[0].size()) + " file(s)");
        Item->setTextAlignment(Qt::AlignCenter);
        table->setItem(i, 1, Item);

        // Paired
        Item = new QTableWidgetItem;
        Item->setText(QString::number(FileLists[1].size()) + " file(s)");
        Item->setTextAlignment(Qt::AlignCenter);
        table->setItem(i, 2, Item);

        // Status
        Item = new QTableWidgetItem;
        Item->setTextAlignment(Qt::AlignCenter);

        QStringList SampleWarnings;
        // Check sample
        if(CoreApplication.CheckIsDuplicated(Sample.ID)){
            SampleWarnings.push_back("Duplicated ID(s)");
            isConfig=false;
        }
        if((FileLists[0].size()+FileLists[1].size()+FileLists[2].size())==0){
            SampleWarnings.push_back("Missing file(s)");
            isConfig=false;
        }
        if(CoreApplication.CheckIsInvalidChar(Sample.ID)){
            SampleWarnings.push_back("Invalid ID char(s)");
            isConfig=false;
        }
        for(QString Lane : Sample.FastqFiles.keys())
            for(QString Mate : Sample.FastqFiles[Lane].keys()){
            if(CoreApplication.CheckIsInexistingFile(Sample.FastqFiles[Lane][Mate])){
                if(!SampleWarnings.contains("Inxesting file(s)"))
                        SampleWarnings.push_back("Inxesting file(s)");
                isConfig=false;
            }
            if(CoreApplication.CheckIsUnmatchingFilename(Lane,Mate,Sample)){
                if(!SampleWarnings.contains("Unmatching file(s)"))
                    SampleWarnings.push_back("Unmatching file(s)");
                isConfig=false;
            }
        }
        Item->setText(SampleWarnings.isEmpty()?"Ready":SampleWarnings.join("; "));
        table->setItem(i, 3, Item);

        if(!SampleWarnings.isEmpty()){
            for(int j(0); j < HorizontalLabels.size(); j++){
                QFont tmpFont = ui->SamplesTableWidget->item(i,j)->font();
                tmpFont.setBold(true);
                ui->SamplesTableWidget->item(i,j)->setFont(tmpFont);
                ui->SamplesTableWidget->item(i,j)->setForeground(QBrush(QColor(255,0,0)));
                ui->SamplesTableWidget->item(i,j)->setToolTip(SampleWarnings.join("; "));
            }
        }
    }
    bool isSizeOk = CoreApplication.GetSampleSize() >=MIN_N;
    ui->TotalSamplesLabel->setText(QString::number(CoreApplication.GetSampleSize()));
    if(isConfig)
        ui->TotalSamplesIcon->setPixmap(QPixmap(isSizeOk?":/etc/checked.png":":/etc/missing.png"));
    else
        ui->TotalSamplesIcon->setPixmap(QPixmap(":/etc/error.png"));
    page1_isConfiguredTable=isConfig && isSizeOk;
    on_SamplesTableWidget_itemSelectionChanged();
    page1_UpdateStartButton();
    return;
}
void MainWindow::page1_UpdateStartButton(){
    ui->StartButton->setEnabled(
                page1_isConfiguredTable &&
                page1_isConfiguredReference &&
                page1_isConfiguredParameters &&
                page1_isConfiguredProjectPath);
}
void MainWindow::page2_AllStepsFinished(){
    ui->page2_CheckStatisticsButton->setEnabled(true);
    ui->page2_ExitButton->setEnabled(true);
}
void MainWindow::page2_UpdateStatus(int WhichStep, int Status){
    QLabel *Label = nullptr;
    switch (WhichStep) {
        case LCSeqTools::AllStep_Finished: page2_AllStepsFinished(); return;
        case LCSeqTools::Step1_BuildIndex: Label=ui->Step1Icon; break;
        case LCSeqTools::Step2_AlignFastq: Label=ui->Step2Icon; break;
        case LCSeqTools::Step3_VariantCall: Label=ui->Step3Icon; break;
        case LCSeqTools::Step4_VcfFiltering: Label=ui->Step4Icon; break;
        case LCSeqTools::Step5_Imputation: Label=ui->Step5Icon; break;
        case LCSeqTools::Step6_Statistics: Label=ui->Step6Icon; break;
        default: break;
    }
    if(Label){
        Label->clear();
        switch (Status) {
            case LCSeqTools::Status_Failed: Label->setPixmap(QPixmap(":/etc/error.png"));; break;
            case LCSeqTools::Status_Waiting:  Label->setPixmap(QPixmap(":/etc/timer.png"));; break;
            case LCSeqTools::Status_Running:  Label->setMovie(new QMovie(":/etc/loader.gif")); Label->movie()->start();; break;
            case LCSeqTools::Status_Finished:  Label->setPixmap(QPixmap(":/etc/checked.png"));; break;
            default: break;
        }
    }
}
void MainWindow::on_StartButton_clicked(){
    page2_UpdateStatus(LCSeqTools::Step1_BuildIndex,LCSeqTools::Status_Waiting);
    page2_UpdateStatus(LCSeqTools::Step2_AlignFastq,LCSeqTools::Status_Waiting);
    page2_UpdateStatus(LCSeqTools::Step3_VariantCall,LCSeqTools::Status_Waiting);
    page2_UpdateStatus(LCSeqTools::Step4_VcfFiltering,LCSeqTools::Status_Waiting);
    page2_UpdateStatus(LCSeqTools::Step5_Imputation,LCSeqTools::Status_Waiting);
    page2_UpdateStatus(LCSeqTools::Step6_Statistics,LCSeqTools::Status_Waiting);
    ui->page2_CheckStatisticsButton->setEnabled(false);
    ui->page2_ExitButton->setEnabled(false);
    ui->page2_GoBackButton->setEnabled(false);
    ui->LogBrowser->clear();
    ui->SystemCallBrowser->clear();
    ChangeCurrentPage(MainWindow_RunningPage);
    //Future = QtConcurrent::run([=](){RunCoreApplication();});
    Future = QtConcurrent::run(this,&MainWindow::RunCoreApplication);
}
void MainWindow::on_page2_CheckStatisticsButton_clicked(){
    StatsWindow *Stats = new StatsWindow(this,&CoreApplication);
    Stats->show();
}
void MainWindow::RunCoreApplication(){
    try {
        CoreApplication.Run();
    } catch (QString ErrorMsg) {
        FailMsgFromCoreApp(ErrorMsg);
    }
}
void MainWindow::FailMsgFromCoreApp(QString ErrorMsg){
    Future.cancel();
    ui->page2_GoBackButton->setEnabled(true);
    emit CoreApplication.LogMsgSignal("<p><strong><span style=\"color: rgb(255, 0, 0);\">" + ErrorMsg + "</span></strong></p>");
}
void MainWindow::ChangeCurrentPage(int CurrentIndex){
    ui->stackedWidget->setCurrentIndex(CurrentIndex);
    ui->NewProjectButton->setEnabled(CurrentIndex==MainWindow_WelcomePage);
    ui->actionOpen_Project_Statistics->setEnabled(CurrentIndex==MainWindow_WelcomePage);
    ui->menuEdit->setEnabled(CurrentIndex==MainWindow_EditingPage);
    ui->menuAnalyze->setEnabled(CurrentIndex==MainWindow_EditingPage);
}
void MainWindow::on_page2_GoBackButton_clicked(){
    ChangeCurrentPage(MainWindow_EditingPage);
    page1_UpdateReference();
    page1_UpdateParameters();
    page1_UpdateProjectPath();
    page1_UpdateTable();
}
void MainWindow::on_page2_ExitButton_clicked(){
    exit(0);
}
void MainWindow::TestMode(){
    CoreApplication.MyProject.ProjectPath="/home/marcus/teste/out";
    for(QString Dir : CoreApplication.Dirs){
        QDir tmpDir(CoreApplication.MyProject.ProjectPath+"/"+Dir);
        tmpDir.removeRecursively();
    }
    CoreApplication.MyProject.FastaReferenceFile="/home/marcus/teste/reference.fa";
    this->show();
    ChangeCurrentPage(MainWindow_EditingPage);
    page1_UpdateReference();
    page1_UpdateParameters();
    page1_UpdateProjectPath();
    page1_UpdateTable();
}
