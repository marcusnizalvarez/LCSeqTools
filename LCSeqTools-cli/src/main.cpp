#include "lcseqtools_cli.h"
int main(int argc, char** argv){
    try{
        LCSeqTools a(argc, argv);
        a.Run();
    }
    catch(QString Msg){
        if(!Msg.isEmpty())
            std::clog << Msg.toStdString() << std::endl;
        return 1;
    }
    return 0;
}
