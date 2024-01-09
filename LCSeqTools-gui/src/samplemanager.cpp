#include "samplemanager.h"
#include "ui_samplemanager.h"
SampleManager::SampleManager(QWidget *parent, SampleStruct* P_Sample, LCSeqTools *P_Core) :
    QWidget(parent),ui(new Ui::SampleManager){
    ui->setupUi(this);
    this->setWindowFlag(Qt::Dialog);
    Sample = P_Sample;
    Core = P_Core;
    ResetName=Sample->ID;
    LaneCount=GetLaneCount();
    UpdateThis();
}
SampleManager::~SampleManager(){
    delete ui;
}
void SampleManager::UpdateThis(){
    ui->SampleNameLineEdit->setText(Sample->ID);
    ui->IgnoreUnmatchingFilenameBox->setChecked(Sample->isIgnoreUnmatchingFilename);
    ui->IsReseqBox->setChecked(Sample->isResequencing);
    LaneLabels.clear();
    for(int i(0); i < LaneCount; i++) LaneLabels.push_back("L00"+QString::number(i+1));
    ui->FilesCountLabel->setText(QString::number(GetFileCount()));
    // Table
    QTableWidget *table = ui->FilesTableWidget;
    table->clear();
    // Rows
    table->setRowCount(LaneLabels.size());
    table->setVerticalHeaderLabels(LaneLabels);
    table->verticalHeader()->setVisible(true);
    // Columns
    table->setColumnCount(MateLabels.size());
    table->setHorizontalHeaderLabels(MateLabels);
    table->horizontalHeader()->setStretchLastSection(true);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->horizontalHeader()->setVisible(true);
    // Fill with empty items
    for(int i (0); i < LaneLabels.size(); i++) for(int j (0); j < MateLabels.size(); j++){
        table->setItem(i,j,new QTableWidgetItem);
        table->item(i,j)->setForeground(QColor(232,232,232));
        table->item(i,j)->setText("<click to edit>");
        table->item(i,j)->setTextAlignment(Qt::AlignCenter);
    }
    for(QString Lane : Sample->FastqFiles.keys()) for(QString Mate : Sample->FastqFiles[Lane].keys()){
        QTableWidgetItem* Item=table->item(LaneLabels.indexOf(Lane),MateLabels.indexOf(Mate));
        Item->setForeground((!Core->CheckIsUnmatchingFilename(Lane,Mate,*Sample))?QColor(0,0,0):QColor(255,0,0));
        Item->setText(QFileInfo(Sample->FastqFiles[Lane][Mate]).fileName());
        Item->setToolTip(Sample->FastqFiles[Lane][Mate]);
    }
}
void SampleManager::on_ResetNameButton_clicked(){
    Sample->ID = ResetName;
    ui->SampleNameLineEdit->setText(Sample->ID);
}
void SampleManager::on_SampleNameLineEdit_textChanged(const QString &arg1){
    int Pos=ui->SampleNameLineEdit->cursorPosition();
    if(arg1.size()==QRegularExpressionMatch(
                QRegularExpression(VALID_ID_PATTERN).match(arg1)).captured().size()){
        Sample->ID = arg1;
    } else if(Pos>0)
        Pos--;
    ui->SampleNameLineEdit->setText(Sample->ID);
    ui->SampleNameLineEdit->setCursorPosition(Pos);
}
void SampleManager::closeEvent(QCloseEvent *event){
    if(event) emit WidgetClosed();
}
void SampleManager::on_OKButton_clicked(){
    this->close();
}
void SampleManager::on_SetFileButton_clicked(){
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
    QString File = QFileDialog::getOpenFileName(this,
                                                ("Select a file"),
                                                LastPath.path(),
                                                "Compressed FASTQ (*.fq.gz *.fastq.gz)");
    if(File.isEmpty()) return;
    LastPath.setPath(File);
    int i = ui->FilesTableWidget->selectedItems().first()->row();
    int j = ui->FilesTableWidget->selectedItems().first()->column();
    Sample->FastqFiles[LaneLabels[i]][MateLabels[j]]=File;
    UpdateThis();
}
void SampleManager::on_RemoveFileButton_clicked(){
    int i = ui->FilesTableWidget->selectedItems().first()->row();
    int j = ui->FilesTableWidget->selectedItems().first()->column();
    Sample->FastqFiles[LaneLabels[i]].remove(MateLabels[j]);
    if(Sample->FastqFiles[LaneLabels[i]].keys().isEmpty()) Sample->FastqFiles.remove(LaneLabels[i]);
    UpdateThis();
}
void SampleManager::on_FilesTableWidget_itemSelectionChanged(){
    if(ui->FilesTableWidget->selectedItems().size()>0){
        ui->SetFileButton->setEnabled(true);
        ui->RemoveFileButton->setEnabled(true);
    }
    else{
        ui->SetFileButton->setEnabled(false);
        ui->RemoveFileButton->setEnabled(false);
    }
}
int SampleManager::GetLaneCount(){
    int Count(1);
    for(QString Key : Sample->FastqFiles.keys()){
        if(Sample->FastqFiles[Key].values().join("").isEmpty()) continue;
        int i=QRegularExpressionMatch(
                    QRegularExpression("L00([1-9])").match(Key)).captured(1).toInt();
        if(Count<i) Count=i;
    }
    return Count;
}
void SampleManager::on_AddLaneButton_clicked(){
    if(LaneCount<9) LaneCount++;
    UpdateThis();
}
void SampleManager::on_RemoveLaneButton_clicked(){
    if(LaneCount<2) return;
    for(auto File : Sample->FastqFiles[LaneLabels.last()].values())
        if(!File.isEmpty()) return;
    LaneCount--;
    UpdateThis();
}
void SampleManager::on_IgnoreUnmatchingFilenameBox_stateChanged(int arg1){
    Sample->isIgnoreUnmatchingFilename=bool(arg1);
    UpdateThis();
}
void SampleManager::on_IsReseqBox_stateChanged(int arg1){
    for(auto& tmpSample : Core->MyProject.SampleList){
        if(tmpSample.ID==Sample->ID)
            tmpSample.isResequencing=arg1;
    }
    UpdateThis();
}
int SampleManager::GetFileCount(){
    int tmpCounter(0);
    for (const auto& Lane : Sample->FastqFiles)
        for (const auto& File : Lane)
            if(!File.isEmpty())
                tmpCounter++;
    return(tmpCounter);
}
