#ifndef LCSEQTOOLS_H
#define LCSEQTOOLS_H
#include <QCoreApplication>
#include <QtCore>
#include <QThread>
#include <QString>
#include <QVector>
#include <QObject>
#include <QMap>
#include <iostream>
#define VALID_ID_PATTERN "^[a-zA-Z0-9_-]*$"
#define ILLUMINA_REGEX "^(.*)_(S[0-9]+?)_(L00[1-9])_(R[1-2]?)_(001.*)$"
#define REFERENCE_PATH "Reference"
#define CONFIG_PATH "Parameters"
#define VCF_PATH "VariantCall"
#define BAM_PATH "Alignment"
#define TEMP_PATH ".temp"
void DebugMsg(QString Msg);
struct ReferencePloidyStruct{
    QString FastaReferenceFile="";
    QStringList SequenceIDs;
    QStringList SequenceDescriptions;
    QList<int> SequencePloidy;
};
struct ParametersStruct{
    int global_maxthreads=QThread::idealThreadCount();
    int trimmomatic_head_crop=10;
    int trimmomatic_trailing_crop=20;
    int trimmomatic_min_len=40;
    int lcvcftools_minGQ=20;
    int lcvcftools_minDP=5;
    double lcvcftools_maf=0.1;
    double lcvcftools_max_missing_variant=0.5;
    int beagle_ImputationSeed=1;
    bool beagle_isLowMemoryEnabled=false;
    double bcftools_minGP=0.95;
    double other_max_missing_sample=0;
};
struct SampleStatsStruct{
    QString ID="";
    QMap<int,double> DP, GQ;
    double NMR=0, MDP=0, MGQ=0;
};
struct SnpStatsStruct{
    int Ts=0, Tv=0;
    double TsTv=0;
    QMap<QString,int> SubstitutionTypes;
};
struct SampleStruct{
    QString ID = "", FID = "";
    bool isIgnoreUnmatchingFilename=false;
    bool isResequencing=false;
    QMap<QString,QMap<QString, QString>> FastqFiles; // <Lane,<Mate,File>>
};
struct Project{
    bool isValidProject = true;
    bool isConfigured=false;
    // ---- Project Parameters ----
    int Threads=QThread::idealThreadCount();
    QList<SampleStruct> SampleList;
    bool isIgnoreUnmatchingFilenames=false;
    QString ConfigFile="";
    QString SampleSheetFile="";
    QString FastaReferenceFile="";
    QString ProjectPath="";
    QString IgnoredSequencesFile="";
    QString HaploidSequencesFile="";
    bool isImputEnable=true;
    // ---- GBS parameters ----
    QMap<QString,QStringList> FlagsList;
    // ---- GUI parameters ----
    ParametersStruct Parameters;
    ReferencePloidyStruct ReferencePloidy;
};
class LCSeqTools : public QObject{
    Q_OBJECT
signals:
    void UpdateStatus(int WhichStep, int Status);
    void LogMsgSignal(QString Msg);
    void SystemCallSignal(QString Msg);
public:
    LCSeqTools();
    LCSeqTools(int argc, char* argv[]);
    void LogMsg(QString Msg);
    void FailMsg(QString Msg);
    void Run();
    void ProjectCheckup();
    void ShowHelp();
    void ShellScript(QString ScriptName, QStringList Arguments);
    void StatusChanged(int WhichStep, int Status);
    void ReadSampleSheet(QString Filename);
    void ReadConfigFile(QString Filename);
    void ReadLcvcftoolsStatsFile(QString Filename = "stats1.tsv");
    SnpStatsStruct ReadBcftoolsStatsFile(QString Filename = "stats2.tsv");
    void WriteSourceFile();
    void WriteReferencePloidyLists();
    bool DeleteExistingProject();
    bool CheckIsParamConfigured();
    bool CheckIsExistingRefFile();
    bool CheckIsValidProjectPath();
    bool CheckIsDuplicated(QString ID);
    bool CheckIsInvalidChar(QString ID);
    bool CheckIsUnmatchingFilename(QString Lane, QString Mate, const SampleStruct& Sample);
    bool CheckIsInexistingFile(QString Filename);
    bool FilterByNonMissingRate();
    void ConvertParametersToFlagList(); // GUI usage
    QList<QStringList> GetFileLists(const SampleStruct& Sample); // return Single,Paired1,Paired2
    QStringList GetIlluminaRegex(QString Filename);
    int GetFileCount(const SampleStruct& Sample);
    int GetSampleSize();
    bool QStringToBool(QString Value, QString ifError);
    int QStringToInt(QString Value, QString ifError);
    double QStringToDouble(QString Value, QString ifError);
    Project MyProject;
    QString AppDir;
    const QString SourceFile="project.cfg";
    const QStringList Dirs = {TEMP_PATH,VCF_PATH,CONFIG_PATH,BAM_PATH,REFERENCE_PATH};
    const QStringList AllowedFlags = {"TRIMMOMATIC","BWA_MEM","SAMTOOLS_VIEW","LCVCFTOOLS","BEAGLE","BCFTOOLS_FILTER"};
    bool isVerbose=false;
    bool isGraphical=true;
    static bool isDebug;
    QMap<QString,SampleStatsStruct> SampleStats;
    typedef enum {
        Uninitialized=-1,
        AllStep_Finished=0,
        Step1_BuildIndex=1,
        Step2_AlignFastq=2,
        Step3_VariantCall=3,
        Step4_VcfFiltering=4,
        Step5_Imputation=5,
        Step6_Statistics=6
    } AppCurrentStep;
    typedef enum {
        Status_Failed=-1,
        Status_Waiting=0,
        Status_Running=1,
        Status_Finished=2
    } AppCurrentStatus;
    int CurrentAppStep=Uninitialized;
    int CurrentAppStatus=Status_Waiting;
};
#endif
