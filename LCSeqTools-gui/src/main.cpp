#include "mainwindow.h"
int main(int argc, char *argv[]){
    QApplication a(argc, argv);
    QStringList args;
    for(int i(1); i < argc; i++) args << argv[i];
    MainWindow w;
    if(args.contains("--debug")) {
        LCSeqTools::isDebug=true;
    }
    if(args.contains("--test")) {
        LCSeqTools::isDebug=true;
        w.TestMode();
        return a.exec();
    }
    w.Splashscreen();
    return a.exec();
}
