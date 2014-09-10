#define SEGGERCMD_AS_MAIN

#include "SeggerCMD.h"

#include <fstream>

void beginSeg(string cate, string inFile, string outFile, string enc,
        string posDelimiter){
	cout<<"Initializing "<<endl;
    CMA_Factory *factory = CMA_Factory::instance();
    Knowledge* knowledge = factory->createKnowledge();
    knowledge->loadModel( enc.data(), cate.data() );

    Analyzer* analyzer = factory->createAnalyzer();
    analyzer->setOption(Analyzer::OPTION_TYPE_POS_TAGGING,0);
    analyzer->setOption(Analyzer::OPTION_ANALYSIS_TYPE,77);
    analyzer->setKnowledge(knowledge);
    analyzer->setPOSDelimiter(posDelimiter.data());

    cout<<"Start Tagging "<<endl;
    VTimer timer("Tag " + inFile);
    timer.start();
    analyzer->runWithStream(inFile.data(), outFile.data());
    timer.endAndPrint();
    
}

void printSeggerUsage(){
    cout<<"SYNOPSIS\n ./cmacsegger modelPath inFile outFile [encoding] [posDelimiter]"<<endl;
    cout<<"Description"<<endl;
    cout<<"   modelPath is the path of the directory that contains all the model, like "<<
            "poc.model, sys.dic and so on."<<endl;
    cout<<"   inFile is the input file"<<endl;
    cout<<"   outFile is the output file"<<endl;
    cout<<"   encoding is the encoding of the mateFile, and utf8 "<<
            "is the default encoding. Support utf8, gb2312, big5 and gb18030 now"<<endl;
    cout<<"   posDelimiter is the delimiter between the word and the pos tag, "<<
            "like '/' and '_' and default is '/'"<<endl;
}

int seggerEntry(int argc, char** argv){
    if(argc < 4){
        printSeggerUsage();
        return 0;
    };
    string cateFile(argv[1]);
    string inFile(argv[2]);
    string outFile(argv[3]);
    string encoding = "utf8";
    if(argc > 4){
        encoding = argv[4];
    }
    string posDelimiter = "/";
    if(argc > 5){
        posDelimiter = argv[5];
    }

    beginSeg(cateFile, inFile, outFile, encoding, posDelimiter);
    return 1;
}

#ifdef SEGGERCMD_AS_MAIN
int main(int argc, char** argv){
    return seggerEntry(argc, argv);
}
#endif
