#include "referenceeditor.h"
#include "ui_referenceeditor.h"
ReferenceEditor::ReferenceEditor(QWidget *parent, LCSeqTools *P_Core) :
    QDialog(parent),
    ui(new Ui::ReferenceEditor){
    Core=P_Core;
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(ReferenceEditor_Loading);
    ui->LoadingIcon->clear();
    ui->LoadingIcon->setMovie(new QMovie(":/etc/loader.gif"));
    ui->LoadingIcon->movie()->start();
    connect(this,&ReferenceEditor::UpdateTableSignal,this,&ReferenceEditor::UpdateTable);
    QtConcurrent::run(this,&ReferenceEditor::GetSequenceIds);
}
ReferenceEditor::~ReferenceEditor(){
    delete ui;
}
void ReferenceEditor::GetSequenceIds(){
    try {
        const QString& FastaFile=Core->MyProject.FastaReferenceFile;
        if(Core->MyProject.ReferencePloidy.FastaReferenceFile==FastaFile) {
            emit UpdateTableSignal();
            return;
        }
        if(!QFile(FastaFile).exists()) {
            GetSequenceIdsFailed("FASTA file does not exists.");
        }
        QString Command;
        if(FastaFile.contains(".gz"))
            Command="gzip -cd '" + FastaFile + "' | grep '>' | cut -c2-";
        else
            Command="cat '" + FastaFile + "' | grep '>' | cut -c2-";
        QProcess SystemCall;
        connect(this,&ReferenceEditor::destroyed,&SystemCall,&QProcess::kill);
        SystemCall.start("bash",QStringList({"-c",Command}));
        SystemCall.waitForFinished(-1);
        SystemCall.waitForReadyRead(-1);
        if(SystemCall.exitCode())
            GetSequenceIdsFailed("Something went wrong while reading from FASTA file.");
        ReferencePloidyStruct tmpReferencePloidy;
        tmpReferencePloidy.FastaReferenceFile=Core->MyProject.FastaReferenceFile;
        QStringList TmpStringList=QString(SystemCall.readAllStandardOutput()).split('\n');
        QStringList TmpIds, TmpDescriptions;
        for(QString TmpString : TmpStringList){
            QStringList TmpSplit=TmpString.split(" ");
            TmpIds.push_back(TmpSplit[0]);
            if(TmpSplit.length()==1){
                TmpDescriptions.push_back("");
            } else {
                TmpSplit.removeAt(0);
                TmpDescriptions.push_back(TmpSplit.join(' '));
            }
        }
        tmpReferencePloidy.SequenceIDs=TmpIds;
        tmpReferencePloidy.SequenceIDs.removeAll("");
        tmpReferencePloidy.SequenceDescriptions=TmpDescriptions;
        for(int i(0); i < tmpReferencePloidy.SequenceIDs.size(); i++) tmpReferencePloidy.SequencePloidy.push_back(2);
        Core->MyProject.ReferencePloidy=tmpReferencePloidy;
        emit UpdateTableSignal();
    } catch (int) {
        return;
    }
}
void ReferenceEditor::GetSequenceIdsFailed(QString Message){
    ui->stackedWidget->setCurrentIndex(ReferenceEditor_Loading);
    ui->LoadingIcon->clear();
    ui->LoadingIcon->setPixmap(QPixmap(":/etc/error.png"));
    ui->LoadingLabel->setText(Message);
    throw 0;
}
void ReferenceEditor::UpdateTable(){
    ui->stackedWidget->setCurrentIndex(ReferenceEditor_Editing);
    QTableWidget *table = ui->tableWidget;
    table->clear();
    // Rows
    table->setRowCount(Core->MyProject.ReferencePloidy.SequenceIDs.size());
    table->verticalHeader()->setVisible(false);
    // Columns
    QStringList HorizontalLabels = QStringList {"Sequence ID","Ploidy","Description"};
    table->setColumnCount(HorizontalLabels.size());
    table->setHorizontalHeaderLabels(HorizontalLabels);
    table->horizontalHeader()->setVisible(true);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
    table->resizeColumnToContents(0);
    table->resizeColumnToContents(1);
    table->setColumnWidth(2,(table->horizontalHeader()->width()-(table->columnWidth(0)+table->columnWidth(1))));
    // Build Table
    for(int i(0); i < table->rowCount(); i++){
        QTableWidgetItem *Item;
        // ID
        Item = new QTableWidgetItem;
        Item->setText(Core->MyProject.ReferencePloidy.SequenceIDs[i]);
        Item->setTextAlignment(Qt::AlignCenter);
        Item->setToolTip(Core->MyProject.ReferencePloidy.SequenceIDs[i]);
        table->setItem(i, 0, Item);
        // Ploidy
        Item = new QTableWidgetItem;
        Item->setText(QString::number(Core->MyProject.ReferencePloidy.SequencePloidy[i]));
        Item->setTextAlignment(Qt::AlignCenter);
        table->setItem(i, 1, Item);
        // Description
        Item = new QTableWidgetItem;
        Item->setText(Core->MyProject.ReferencePloidy.SequenceDescriptions[i]);
        Item->setTextAlignment(Qt::AlignVCenter);
        Item->setToolTip(Core->MyProject.ReferencePloidy.SequenceDescriptions[i]);
        table->setItem(i, 2, Item);
    }
    on_tableWidget_itemSelectionChanged();
}
void ReferenceEditor::on_tableWidget_itemSelectionChanged(){
    bool tmpBool = ui->tableWidget->selectedItems().size() > 0;
    ui->DiploidButton->setEnabled(tmpBool);
    ui->HaploidButton->setEnabled(tmpBool);
    ui->IgnoredButton->setEnabled(tmpBool);
}
void ReferenceEditor::on_CloseButton_clicked(){
    this->close();
}
void ReferenceEditor::on_DiploidButton_clicked(){
    SetPloidy(2);
    UpdateTable();
}
void ReferenceEditor::on_HaploidButton_clicked(){
    SetPloidy(1);
    UpdateTable();
}
void ReferenceEditor::on_IgnoredButton_clicked(){
    SetPloidy(0);
    UpdateTable();
}
void ReferenceEditor::SetPloidy(int Ploidy){
    QItemSelectionModel *Selected = ui->tableWidget->selectionModel();
    if(!Selected->hasSelection()) return;
    auto SelectedRows=Selected->selectedRows();
    for(int i(0); i < SelectedRows.size(); i++)
        Core->MyProject.ReferencePloidy.SequencePloidy[SelectedRows[i].row()]=Ploidy;
    UpdateTable();
}
QStringList ReferenceEditor::ImportList(){
    QStringList StringList;
    QString Filename = QFileDialog::getOpenFileName(this,
                                                    "Select a list file (raw text)",
                                                    QDir::homePath(),
                                                    tr("List file (*)"));
    if(Filename.isEmpty()) return StringList;
    QFile File (Filename);
    if(!File.exists()){
        QMessageBox::critical(this,"Error","List file does not exists.",QMessageBox::Ok);
        return QStringList();
    }
    if(!File.open(QIODevice::ReadOnly)){
        QMessageBox::critical(this,"Error","List file not readable.",QMessageBox::Ok);
        return QStringList();
    }
    QTextStream Stream(&File);
    while(!Stream.atEnd()){
        QString Line=Stream.readLine();
        if(Line[0]=='#') continue;
        StringList.push_back(Line);
    }
    return StringList;
}
void ReferenceEditor::on_ImportDiploidButton_clicked(){
    for(auto String : ImportList()){
        int i = Core->MyProject.ReferencePloidy.SequenceIDs.indexOf(String);
        if(i>=0) Core->MyProject.ReferencePloidy.SequencePloidy[i]=2;
    }
    UpdateTable();
}
void ReferenceEditor::on_ImportHaploidButton_clicked(){
    for(auto String : ImportList()){
        int i = Core->MyProject.ReferencePloidy.SequenceIDs.indexOf(String);
        if(i>=0) Core->MyProject.ReferencePloidy.SequencePloidy[i]=1;
    }
    UpdateTable();
}
void ReferenceEditor::on_ImportIgnoredButton_clicked(){
    for(auto String : ImportList()){
        int i = Core->MyProject.ReferencePloidy.SequenceIDs.indexOf(String);
        if(i>=0) Core->MyProject.ReferencePloidy.SequencePloidy[i]=0;
    }
    UpdateTable();
}
