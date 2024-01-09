#include "lcseqtools_cli.h"
LCSeqTools::LCSeqTools(){
}
LCSeqTools::LCSeqTools(int argc, char* argv[]){
    // **** This function is used for CLI version only ****
    if(argc==1){
        LogMsg("LCSeqTools v" + QString(GIT_REVISION) + ". Run with --help for more information.");
        throw QString("");
    }
    QVector<QString> args;
    for(int i(1); i < argc; i++) args.push_back(QString(argv[i]));
    // --------------------------- Set Parameters
    isGraphical=false;
    QVector<QString> tmp_DefinedArgs;
    for(int i(0); i < args.size(); i++){
        // --------------------------- Required parameters
        if(args[i]=="--out"){
            tmp_DefinedArgs.push_back(args[i]);
            if(tmp_DefinedArgs.count(args[i])>1) break;
            if(++i >= args.size()) FailMsg("Missing argument value for --out");
            MyProject.ProjectPath = QDir(args[i]).absolutePath();
            continue;
        }
        if(args[i]=="--fasta"){
            tmp_DefinedArgs.push_back(args[i]);
            if(tmp_DefinedArgs.count(args[i])>1) break;
            if(++i >= args.size()) FailMsg("Missing argument value for --fasta");
            MyProject.FastaReferenceFile = QFileInfo(args[i]).absoluteFilePath();
            continue;
        }
        if(args[i]=="--sample-sheet"){
            tmp_DefinedArgs.push_back(args[i]);
            if(tmp_DefinedArgs.count(args[i])>1) break;
            if(++i >= args.size()) FailMsg("Missing argument value for --sample-sheet");
            MyProject.SampleSheetFile=QFileInfo(args[i]).absoluteFilePath();
            continue;
        }
        // --------------------------- Opicional parameters
        if(args[i]=="--config"){
            tmp_DefinedArgs.push_back(args[i]);
            if(tmp_DefinedArgs.count(args[i])>1) break;
            if(++i >= args.size()) FailMsg("Missing argument value for --config");
            MyProject.ConfigFile=QFileInfo(args[i]).absoluteFilePath();
            continue;
        }
        if(args[i]=="--threads"){
            tmp_DefinedArgs.push_back(args[i]);
            if(tmp_DefinedArgs.count(args[i])>1) break;
            if(++i >= args.size()) FailMsg("Missing argument value for --threads");
            bool Ok;
            int tmpInt = args[i].toInt(&Ok);
            if(!Ok||tmpInt<1||tmpInt>QThread::idealThreadCount())
                FailMsg("Argument for --threads must be an integer between 1 and " + QString::number(QThread::idealThreadCount()));
            MyProject.Threads = tmpInt;
            continue;
        }
        if(args[i]=="--verbose"){
            tmp_DefinedArgs.push_back(args[i]);
            if(tmp_DefinedArgs.count(args[i])>1) break;
            isVerbose = true;
            continue;
        }
        if(args[i]=="--no-imput"){
            tmp_DefinedArgs.push_back(args[i]);
            if(tmp_DefinedArgs.count(args[i])>1) break;
            MyProject.isImputEnable = false;
            continue;
        }
        if(args[i]=="--haploid-seqs"){
            tmp_DefinedArgs.push_back(args[i]);
            if(tmp_DefinedArgs.count(args[i])>1) break;
            if(++i >= args.size()) FailMsg("Missing argument value for --haploid-seqs");
            MyProject.HaploidSequencesFile=QFileInfo(args[i]).absoluteFilePath();
            continue;
        }
        if(args[i]=="--ignored-seqs"){
            tmp_DefinedArgs.push_back(args[i]);
            if(tmp_DefinedArgs.count(args[i])>1) break;
            if(++i >= args.size()) FailMsg("Missing argument value for --ignored-seqs");
            MyProject.IgnoredSequencesFile=QFileInfo(args[i]).absoluteFilePath();
            continue;
        }
        if(args[i]=="--ignore-filename"){
            tmp_DefinedArgs.push_back(args[i]);
            if(tmp_DefinedArgs.count(args[i])>1) break;
            MyProject.isIgnoreUnmatchingFilenames = true;
            continue;
        }
        if(args[i]=="--appdir"){
            tmp_DefinedArgs.push_back(args[i]);
            if(tmp_DefinedArgs.count(args[i])>1) break;
            if(++i >= args.size()) FailMsg("Missing argument value for --appdir");
            AppDir=args[i];
            continue;
        }
        if(args[i]=="--help"){
            ShowHelp();
            throw QString("");
        }
    FailMsg(args[i] + " is an invalid argument");
    }
    // --------------------------- Check for multiple definitions
    for(const QString& tmpArg : tmp_DefinedArgs)
        if(tmp_DefinedArgs.count(tmpArg)>1)
            FailMsg("Multiple " + tmpArg + " definitions");
    // --------------------------- Check for required
    QString tmp_RequiredArgs;
    for(const QString& tmpArg : QVector<QString>({"--out","--fasta","--sample-sheet"}))
        if(!tmp_DefinedArgs.contains(tmpArg))
            tmp_RequiredArgs += tmpArg + "; ";
    if(!tmp_RequiredArgs.isEmpty()) FailMsg("Missing required argument(s): " + tmp_RequiredArgs);
    // --------------------------- Read files
    ReadSampleSheet(MyProject.SampleSheetFile);
    ReadConfigFile(MyProject.ConfigFile);
    if(!MyProject.HaploidSequencesFile.isEmpty()){
        QFile File(MyProject.HaploidSequencesFile);
        if(!File.exists())
            FailMsg("haploid-seqs file does not exists.");
        if(!File.copy(MyProject.ProjectPath+"/"+CONFIG_PATH+"/haploid.list"))
            FailMsg("Failed to copy haploid-seqs file.");
    }
    if(!MyProject.IgnoredSequencesFile.isEmpty()){
        QFile File(MyProject.IgnoredSequencesFile);
        if(!File.exists())
            FailMsg("ignored-seqs file does not exists.");
        if(!File.copy(MyProject.ProjectPath+"/"+CONFIG_PATH+"/ignored.list"))
            FailMsg("Failed to copy ignored-seqs file.");
    }
}
void LCSeqTools::LogMsg(QString Msg){
    Msg="[LCSeqTools] " + Msg;
    emit(LogMsgSignal(Msg));
    std::clog << Msg.toStdString() << std::endl;
}
void LCSeqTools::FailMsg(QString Msg){
    MyProject.isValidProject=false;
    Msg="[LCSeqTools] CRITICAL: " + Msg;
    StatusChanged(CurrentAppStep,Status_Failed);
    throw Msg;
}
void DebugMsg(QString Msg){
    if(LCSeqTools::isDebug) qDebug() << "[LCSeqTools] DEBUG: " << Msg;
}
bool LCSeqTools::isDebug=false;
void LCSeqTools::ShowHelp(){
    QFile File(":/etc/help.txt");
    if(!File.open(QIODevice::ReadOnly)) FailMsg("Something went wrong (help.txt.)");
    QString msg=File.readAll();
    msg.replace("$VERSION",GIT_REVISION);
    msg.replace("$MAX_CPU",QString::number(QThread::idealThreadCount()));
    std::cout << msg.toStdString() << std::endl;
}
void LCSeqTools::ReadConfigFile(QString Filename){
    if(Filename.isEmpty()){
        LogMsg("Using default configuration file.");
        Filename=":/etc/default_config.txt";
    }
    QFile File (Filename);
    if(!File.exists()) FailMsg("Config file does not exists");
    if(!File.open(QIODevice::ReadOnly)) FailMsg("Config file not readable.");
    QTextStream Stream(&File);
    while(!Stream.atEnd()){
        QString Line=Stream.readLine();
        if(Line[0]=='#') continue;
        if(!Line.contains(": ")) break;
        QStringList tmp_StringList=Line.split(": ");
        if(!AllowedFlags.contains(tmp_StringList[0])) FailMsg(tmp_StringList[0] + " is not a valid flag");
        for(QString tmpString : tmp_StringList[1].split(' '))
            MyProject.FlagsList[tmp_StringList[0]].push_back(tmpString);
    }
    MyProject.isConfigured=true;
    LogMsg("Configuration file loaded successfully.");
}
void LCSeqTools::ReadSampleSheet(QString Filename){
    QFile File (Filename);
    QDir FileDir=QFileInfo(Filename).dir();
    if(!File.exists()) FailMsg("Sample-sheet file does not exists");
    if(!File.open(QIODevice::ReadOnly)) FailMsg("Sample-sheet file not readable.");
    QTextStream Stream(&File);
    bool isHeader=true;
    QStringList MateHeader, LaneHeader;
    int tmpLineCounter=0;
    while(!Stream.atEnd()){
        tmpLineCounter++;
        QString Line=Stream.readLine(); // Test with trimmed
        if(Line.isEmpty()) break;
        if(!Line.contains(',')) FailMsg("No comma character at line " + QString(tmpLineCounter));
        if(isHeader){
            QStringList tmpStringList=Line.split(",");
            tmpStringList.removeFirst();
            for(QString tmpString : tmpStringList){
                QStringList tmpStringList2 = tmpString.split('_');
                if(!QRegularExpression("^R[1-2]$").match(tmpStringList2[0]).hasMatch())
                    FailMsg("Invalid sample-sheet header format. Mate code: " + tmpStringList2[0]);
                if(!QRegularExpression("^L00[1-9]$").match(tmpStringList2[1]).hasMatch())
                    FailMsg("Invalid sample-sheet header format. Lane code: " + tmpStringList2[1]);
                MateHeader.push_back(tmpStringList2[0]);
                LaneHeader.push_back(tmpStringList2[1]);
            }
            isHeader=false;
            continue;
        }
        SampleStruct tmp_Sample;
        QStringList Values=Line.split(",");
        tmp_Sample.ID=Values[0];
        Values.removeFirst();
        for(int i(0); i < Values.length(); i++)
            if(!Values[i].isEmpty()){
                tmp_Sample.FastqFiles[LaneHeader[i]][MateHeader[i]]=FileDir.absolutePath() + '/' + Values[i];
            }
        tmp_Sample.isIgnoreUnmatchingFilename=MyProject.isIgnoreUnmatchingFilenames;
        MyProject.SampleList.push_back(std::move(tmp_Sample));
    }
}
bool LCSeqTools::CheckIsDuplicated(QString ID){
    QVector<QString> tmp_SampleNames;
    for(const auto& Sample : MyProject.SampleList){
        if(tmp_SampleNames.contains(Sample.ID) && Sample.isResequencing) continue;
        tmp_SampleNames.push_back(Sample.ID);
    }
    return (tmp_SampleNames.count(ID)>1);
}
bool LCSeqTools::CheckIsInvalidChar(QString ID){
    if(ID.isEmpty()) return true;
    return (!QRegularExpressionMatch(QRegularExpression(VALID_ID_PATTERN).match(ID)).hasMatch());
}
bool LCSeqTools::CheckIsUnmatchingFilename(QString Lane, QString Mate, const SampleStruct& Sample){
    if(Sample.isIgnoreUnmatchingFilename) return false;
    QStringList Match = GetIlluminaRegex(QFileInfo(Sample.FastqFiles[Lane][Mate]).fileName());
    if(Match.isEmpty()) return true;
    if(!Match[2].contains(Lane)) return true;
    if(!Match[3].contains(Mate)) return true;
    return false;
}
bool LCSeqTools::CheckIsParamConfigured(){ // TODO: Implement this
    return MyProject.isConfigured;
}
bool LCSeqTools::CheckIsExistingRefFile(){
    return QFile(MyProject.FastaReferenceFile).exists();
}
bool LCSeqTools::CheckIsInexistingFile(QString Filename){
    return !QFile(Filename).exists();
}
bool LCSeqTools::CheckIsValidProjectPath(){
    if(!QDir(MyProject.ProjectPath).exists()) return false;
    if(!QDir(MyProject.ProjectPath).isReadable()) return false;
    for(QString Dir : Dirs) if(QDir(MyProject.ProjectPath).exists(Dir)) return false;
    return true;
}
bool LCSeqTools::DeleteExistingProject(){
    for(QString Dir : Dirs){
        if(QDir(MyProject.ProjectPath).exists(Dir)){
            QString Command=QString("rm -rf '") + MyProject.ProjectPath + '/' + Dir + "/'";
            int ExitVal=system(Command.toStdString().c_str());
            if(ExitVal) return false;
        }
    }
    return true;
}
QStringList LCSeqTools::GetIlluminaRegex(QString Filename){
    QStringList returnVal;
    QRegularExpressionMatch Match = QRegularExpression(ILLUMINA_REGEX).match(QFileInfo(Filename).fileName());
    if(!Match.hasMatch()) return returnVal;
    returnVal.push_back(Match.captured(1));
    returnVal.push_back(Match.captured(2));
    returnVal.push_back(Match.captured(3));
    returnVal.push_back(Match.captured(4));
    returnVal.push_back(Match.captured(5));
    return returnVal;
}
int LCSeqTools::GetFileCount(const SampleStruct &Sample){
    int returnVal=0;
    for(const auto& Lane : Sample.FastqFiles.keys())
        for(const auto& Mate : Sample.FastqFiles[Lane].keys())
            if(!Sample.FastqFiles[Lane][Mate].isEmpty()) returnVal++;
    return returnVal;
}
int LCSeqTools::GetSampleSize(){
    QStringList uniqueIDs, ReseqIDs;
    for(const auto& Sample : MyProject.SampleList){
        if(Sample.isResequencing){
            if(!ReseqIDs.contains(Sample.ID))
                ReseqIDs.push_back(Sample.ID);
        } else uniqueIDs.push_back(Sample.ID);
    }
    return uniqueIDs.size()+ReseqIDs.size();
}
QList<QStringList> LCSeqTools::GetFileLists(const SampleStruct &Sample){
    QList<QStringList> tmpList;
    QStringList SE, PE_1, PE_2;
    for(const auto& Lane : Sample.FastqFiles){
        if(!Lane["R1"].isEmpty()&&!Lane["R2"].isEmpty()){
            PE_1.push_back(Lane["R1"]+';');
            PE_2.push_back(Lane["R2"]+';');
            continue;
        }
        if(!Lane["R1"].isEmpty())
            SE.push_back(Lane["R1"]+';');
        if(!Lane["R2"].isEmpty())
            SE.push_back(Lane["R2"]+';');
    }
    tmpList.push_back(SE);
    tmpList.push_back(PE_1);
    tmpList.push_back(PE_2);
    return tmpList;
}

bool LCSeqTools::QStringToBool(QString Value, QString ifError){
    if(!Value.compare("false")) return false; // if false
    else if(Value.compare("true")) FailMsg(ifError + " must be a boolean value."); // if not true
    return true;
}
int LCSeqTools::QStringToInt(QString Value, QString ifError){
    bool isOk;
    int returnVal = Value.toInt(&isOk);
    if(!isOk) FailMsg(ifError + " must be an integer value");
    return returnVal;
}
double LCSeqTools::QStringToDouble(QString Value, QString ifError){
    bool isOk;
    double returnVal = Value.toDouble(&isOk);
    if(!isOk) FailMsg(ifError + " must be a decimal value");
    return returnVal;
}
void LCSeqTools::ShellScript(QString ScriptName, QStringList Arguments){
    QFile ScriptFile(":/bash/" + ScriptName);
    QTemporaryFile *tmpScriptFile = QTemporaryFile::createNativeFile(ScriptFile);
    Arguments.insert(0,tmpScriptFile->fileName());
    QProcess SystemCall;
    SystemCall.setProcessChannelMode(isVerbose?QProcess::ForwardedChannels:QProcess::SeparateChannels);
    if(isGraphical) {
        SystemCall.setProcessChannelMode(QProcess::MergedChannels);
        connect(&SystemCall,&QProcess::readyRead,[&]() {
            emit SystemCallSignal(SystemCall.readAll());
        });
    }
    SystemCall.start("/bin/bash",Arguments);
    SystemCall.waitForFinished(-1);
    tmpScriptFile->remove();
    if(SystemCall.exitCode()){
        emit SystemCallSignal(SystemCall.readAll());
        FailMsg("Something went wrong with " + ScriptName + " (code " + QString::number(SystemCall.exitCode()) + ")");
    }
}
void LCSeqTools::ProjectCheckup(){
    QStringList ErrorList;
    auto PushError=[&](QString Msg){MyProject.isValidProject=false;ErrorList.push_back(Msg);return;};
    // --------------------------- Reset isValidProject for each run
    MyProject.isValidProject=true;
    // --------------------------- Check Output folder
    if(!CheckIsValidProjectPath())
        PushError("Project path must be an existing writable empty folder");
    // --------------------------- Check FASTA reference
    if(!CheckIsExistingRefFile())
        PushError("Reference file does not exists");
    // --------------------------- Check Sample-sheet
    if(MyProject.SampleList.size()==0)
        PushError("Empty sample list");
    // --------------------------- Check Param Configured
    if(!CheckIsParamConfigured())
        PushError("Invalid parameters configuration");
    // --------------------------- Check Sample data
    for(const auto& Sample : MyProject.SampleList){
        if(CheckIsDuplicated(Sample.ID))
            PushError(Sample.ID + " is duplicated ID(s)");
        if(CheckIsInvalidChar(Sample.ID))
            PushError("Sample '" + Sample.ID + "' contains invalid character(s)");
        if(GetFileCount(Sample)==0)
            PushError(Sample.ID + " have no associated file");
        for(const auto& Lane : Sample.FastqFiles.keys())
            for(const auto& Mate : Sample.FastqFiles[Lane].keys()){
                QString Filename=Sample.FastqFiles[Lane][Mate];
                if(CheckIsUnmatchingFilename(Lane,Mate,Sample))
                    PushError(Sample.ID + " unmatching with " + Lane + "_" + Mate + " pattern: " + QFileInfo(Filename).fileName());
                if(CheckIsInexistingFile(Filename))
                    PushError(Sample.ID + " inexisting file: " + QFileInfo(Filename).fileName());
            }
    }
    if(!MyProject.isValidProject) {
        for(QString Error : ErrorList) LogMsg("FATAL ERROR! " + Error);
        FailMsg("Aborted.");
    }
}
void LCSeqTools::WriteSourceFile(){
    if(!QDir(MyProject.ProjectPath).mkdir(CONFIG_PATH)) return;
    QFile Filename(MyProject.ProjectPath + "/" + CONFIG_PATH + "/" + SourceFile);
        if (Filename.open(QIODevice::ReadWrite)) {
            QTextStream Stream(&Filename);
            //Stream << "APPDIR=" << AppDir << endl;
            //Stream << "PATH=$APPDIR/usr/bin:$PATH" << endl;
            Stream << "PROJECT_PATH=\"" << MyProject.ProjectPath << "\"" << endl;
            Stream << "TEMP_PATH=\"$PROJECT_PATH/" << TEMP_PATH << "\"" << endl;
            Stream << "CONFIG_PATH=\"$PROJECT_PATH/" << CONFIG_PATH << "\"" << endl;
            Stream << "REFERENCE_PATH=\"$PROJECT_PATH/" << REFERENCE_PATH << "\"" << endl;
            Stream << "BAM_PATH=\"$PROJECT_PATH/" << BAM_PATH << "\"" << endl;
            Stream << "VCF_PATH=\"$PROJECT_PATH/" << VCF_PATH << "\"" << endl;
            Stream << "THREADS=\"" << MyProject.Threads << "\"" << endl;
            Stream << "FASTA_REF=\"" << MyProject.FastaReferenceFile << "\"" << endl;
            for(QString Key : MyProject.FlagsList.keys()){
                Stream << Key + "_FLAGS=(\"" << MyProject.FlagsList[Key].join("\" \"") << "\")" << endl;
            }
        }
}
void LCSeqTools::WriteReferencePloidyLists(){
    if(MyProject.ReferencePloidy.SequencePloidy.contains(1)){
        QFile Filename(MyProject.ProjectPath + "/" + CONFIG_PATH + "/haploid.list");
            if (Filename.open(QIODevice::ReadWrite)) {
                QTextStream Stream(&Filename);
                for(int i(0); i < MyProject.ReferencePloidy.SequencePloidy.size(); i++)
                    if(MyProject.ReferencePloidy.SequencePloidy[i]==1)
                        Stream << MyProject.ReferencePloidy.SequenceIDs[i] << endl;
            }
    }
    if(MyProject.ReferencePloidy.SequencePloidy.contains(0)){
        QFile Filename(MyProject.ProjectPath + "/" + CONFIG_PATH + "/ignored.list");
            if (Filename.open(QIODevice::ReadWrite)) {
                QTextStream Stream(&Filename);
                for(int i(0); i < MyProject.ReferencePloidy.SequencePloidy.size(); i++)
                    if(MyProject.ReferencePloidy.SequencePloidy[i]==0)
                        Stream << MyProject.ReferencePloidy.SequenceIDs[i] << endl;
            }
    }
}
void LCSeqTools::ConvertParametersToFlagList(){
    QMap<QString,QStringList> tmpFlagsList;
    tmpFlagsList["TRIMMOMATIC"] += "HEADCROP:" + QString::number(MyProject.Parameters.trimmomatic_head_crop);
    tmpFlagsList["TRIMMOMATIC"] += "TRAILING:" + QString::number(MyProject.Parameters.trimmomatic_trailing_crop);
    tmpFlagsList["TRIMMOMATIC"] += "MINLEN:"   + QString::number(MyProject.Parameters.trimmomatic_min_len);
    tmpFlagsList["LCVCFTOOLS"] += QStringList({"--minGQ",QString::number(MyProject.Parameters.lcvcftools_minGQ)});
    tmpFlagsList["LCVCFTOOLS"] += QStringList({"--minDP",QString::number(MyProject.Parameters.lcvcftools_minDP)});
    tmpFlagsList["LCVCFTOOLS"] += QStringList({"--minDPR","1",QString::number(MyProject.Parameters.lcvcftools_max_missing_variant)});
    tmpFlagsList["LCVCFTOOLS"] += QStringList({"--MAF",QString::number(MyProject.Parameters.lcvcftools_maf)});
    if(MyProject.Parameters.beagle_isLowMemoryEnabled)
        tmpFlagsList["BEAGLE"] += "lowmem=true";
    else
        tmpFlagsList["BEAGLE"] += "lowmem=false";
    tmpFlagsList["BEAGLE"] += "seed=" + QString::number(MyProject.Parameters.beagle_ImputationSeed);
    tmpFlagsList["BCFTOOLS_FILTER"] += QStringList({"--include", QString("FORMAT/GP>=") + QString::number(MyProject.Parameters.bcftools_minGP)});
    MyProject.FlagsList=tmpFlagsList;
}
void LCSeqTools::ReadLcvcftoolsStatsFile(QString Filename){
    SampleStats.clear();
    QFile File (MyProject.ProjectPath + "/" + VCF_PATH + "/" + Filename);
    if(!File.exists()) FailMsg("LCVCFtools Stats file does not exists.");
    if(!File.open(QIODevice::ReadOnly)) FailMsg("(LCVCFtools Stats file not readable.");
    QTextStream Stream(&File);
    bool isHeader=true;
    while(!Stream.atEnd()){
        QString Line=Stream.readLine();
        if(Line.contains('#')) continue;
        if(isHeader){
            isHeader=false;
            continue;
        }
        QStringList tmpStringList = Line.split('\t');
        if(tmpStringList[3]=="-nan") tmpStringList[3]="0"; // TODO: Keep this?
        SampleStats[tmpStringList[0]].ID=tmpStringList[0];
        if(tmpStringList[1]=="NMR"){
            bool isOk=true;
            SampleStats[tmpStringList[0]].NMR=tmpStringList[3].toDouble(&isOk);
            if(!isOk) FailMsg("Failed to convert value from Stats file");
            continue;
        } else if(tmpStringList[1]=="MDP"){
            bool isOk=true;
            SampleStats[tmpStringList[0]].MDP=tmpStringList[3].toDouble(&isOk);
            if(!isOk) FailMsg("Failed to convert value from Stats file");
            continue;
        } else if(tmpStringList[1]=="MGQ"){
            bool isOk=true;
            SampleStats[tmpStringList[0]].MGQ=tmpStringList[3].toDouble(&isOk);
            if(!isOk) FailMsg("Failed to convert value from Stats file");
            continue;
        } else if(tmpStringList[1]=="DP"){
            bool isOk1=true;
            bool isOk2=true;
            SampleStats[tmpStringList[0]].DP[tmpStringList[2].toInt(&isOk2)]=tmpStringList[3].toDouble(&isOk1);
            if(!isOk1||!isOk2) FailMsg("Failed to convert value from Stats file");
            continue;
        } else if(tmpStringList[1]=="GQ"){
            bool isOk1=true;
            bool isOk2=true;
            SampleStats[tmpStringList[0]].GQ[tmpStringList[2].toInt(&isOk2)]=tmpStringList[3].toDouble(&isOk1);
            if(!isOk1||!isOk2) FailMsg("Failed to convert value from Stats file");
            continue;
        }
    }
}
SnpStatsStruct LCSeqTools::ReadBcftoolsStatsFile(QString Filename){
    QFile File (MyProject.ProjectPath + "/" + VCF_PATH + "/" + Filename);
    if(!File.exists()) FailMsg("BCFtools Stats file does not exists");
    if(!File.open(QIODevice::ReadOnly)) FailMsg("BCFtools Stats file not readable.");
    QTextStream Stream(&File);
    SnpStatsStruct SnpStats;
    while(!Stream.atEnd()){
        QString Line=Stream.readLine();
        if(Line.contains('#')) continue;
        QStringList StringList = Line.split('\t');
        if(StringList.isEmpty()) continue;
        if(StringList[0]=="TSTV"){
            // Ts ----------------------------
            bool isOk=true;
            SnpStats.Ts=StringList[2].toInt(&isOk);
            if(!isOk) FailMsg("Failed to convert value from Stats file");
            // Tv ----------------------------
            SnpStats.Tv=StringList[3].toInt(&isOk);
            if(!isOk) FailMsg("Failed to convert value from Stats file");
            // TsTv --------------------------
            SnpStats.TsTv=StringList[4].toDouble(&isOk);
            if(!isOk) FailMsg("Failed to convert value from Stats file");
        } else if(StringList[0]=="ST"){
            bool isOk=true;
            SnpStats.SubstitutionTypes[StringList[2]]=StringList[3].toInt(&isOk);
            if(!isOk) FailMsg("Failed to convert value from Stats file");
        }
    }
    return SnpStats;
}
bool LCSeqTools::FilterByNonMissingRate(){
    ReadLcvcftoolsStatsFile();
    QStringList BlackList;
    for(const auto& Sample : SampleStats){
        if((Sample.NMR) < (1-MyProject.Parameters.other_max_missing_sample)) {
            LogMsg(Sample.ID + " will be removed. Too much missing data (" + QString::number((int)std::ceil((1-Sample.NMR)*100)) + "%)");
            BlackList.push_back(Sample.ID);
        }
    }
    if(BlackList.empty()) return false;
    QFile Filename(MyProject.ProjectPath + "/" + VCF_PATH + "/2-clean.removed");
    if (Filename.open(QIODevice::ReadWrite)) {
        QTextStream Stream(&Filename);
        for(QString ID : BlackList) Stream << ID << endl;
    }
    return true;
}
void LCSeqTools::StatusChanged(int WhichStep, int Status){
    CurrentAppStep=WhichStep;
    CurrentAppStatus=Status;
    emit UpdateStatus(WhichStep,Status);
}
void LCSeqTools::Run(){
    AppDir=QString(qgetenv("APPDIR")) + "/";
    ProjectCheckup();
    WriteSourceFile();
    // --------------------------- GUI stuff
    if(isGraphical){
        WriteReferencePloidyLists();
    }
    // --------------------------- Step1: Build index
    StatusChanged(Step1_BuildIndex,Status_Running);
    LogMsg("Building FASTA reference index...");
    ShellScript("build_index.sh",QStringList{
                    MyProject.ProjectPath + "/" + CONFIG_PATH + "/" + SourceFile
                });
    StatusChanged(Step1_BuildIndex,Status_Finished);
    // --------------------------- Step2: Aligment
    StatusChanged(Step2_AlignFastq,Status_Running);
    for(const SampleStruct& Sample : MyProject.SampleList){
        QList<QStringList> FileLists = GetFileLists(Sample); // 0=Single, 1=Mate_1, 2=Mate_2
        LogMsg("Executing sequence alignment for " + Sample.ID + " [" +
               "Single files=" + QString::number(FileLists[0].size()) + "; " +
               "Paired files=" + QString::number(FileLists[1].size() + FileLists[2].size()) + "]...");
        ShellScript("align_fastq.sh",QStringList{
                        MyProject.ProjectPath + "/" + CONFIG_PATH + "/" + SourceFile,
                        Sample.ID,
                        FileLists[0].join(""),
                        FileLists[1].join(""),
                        FileLists[2].join("")
                    });
    }
    StatusChanged(Step2_AlignFastq,Status_Finished);
    // --------------------------- Step3: Variant call
    StatusChanged(Step3_VariantCall,Status_Running);
    LogMsg("Executing variant calling...");
    ShellScript("variant_call.sh",QStringList{
                    MyProject.ProjectPath + "/" + CONFIG_PATH + "/" + SourceFile
                });
    StatusChanged(Step3_VariantCall,Status_Finished);
    // --------------------------- Step4: Variant filtering
    StatusChanged(Step4_VcfFiltering,Status_Running);
    LogMsg("Executing variant filtering...");
    ShellScript("vcf_filter.sh",QStringList{
                    MyProject.ProjectPath + "/" + CONFIG_PATH + "/" + SourceFile
                });
    if(FilterByNonMissingRate()){
        LogMsg("Executing variant filtering (step two)...");
        ShellScript("vcf_filter.sh",QStringList{
                        MyProject.ProjectPath + "/" + CONFIG_PATH + "/" + SourceFile
                    });
    }
    StatusChanged(Step4_VcfFiltering,Status_Finished);
    // --------------------------- Step5: Imputation
    StatusChanged(Step5_Imputation,Status_Running);
    if(MyProject.isImputEnable){
        LogMsg("Executing imputation...");
        ShellScript("imputation.sh",QStringList{
                        MyProject.ProjectPath + "/" + CONFIG_PATH + "/" + SourceFile
                    });
    }
    else {
        LogMsg("Skipping imputation...");
    }
    StatusChanged(Step5_Imputation,Status_Finished);
    // --------------------------- Step5: Imputation
    StatusChanged(Step6_Statistics,Status_Running);
    LogMsg("Executing Stats...");
    ShellScript("statistics.sh",QStringList{
                    MyProject.ProjectPath + "/" + CONFIG_PATH + "/" + SourceFile
                });
    StatusChanged(Step6_Statistics,Status_Finished);
    // --------------------------- FINISH
    LogMsg("Finish successfully.");
    StatusChanged(AllStep_Finished,Status_Finished);
}
