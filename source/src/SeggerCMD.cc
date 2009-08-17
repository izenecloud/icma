#define SEGGERCMD_AS_MAIN

#include "SeggerCMD.h"

#include <fstream>

void beginSeg(string cate, string inFile, string outFile, string enc,
        string posDelimiter){
    Knowledge::EncodeType encType = Knowledge::decodeEncodeType(enc.c_str());
    
    string poc_mate = cate + "-poc";

    CMA_Factory *factory = CMA_Factory::instance();
    Knowledge* knowledge = factory->createKnowledge();

    knowledge->setEncodeType(encType);

    cout<<"Create Analyzer "<<endl;
    Analyzer* analyzer = factory->createAnalyzer();
    analyzer->setOption(Analyzer::OPTION_TYPE_POS_TAGGING, 1);

    cout<<"Load Stat Model "<<endl;
    knowledge->loadStatModel(poc_mate.data());
    //check whether exist pos model
	ifstream posIn((cate + ".model").data());
	if(posIn)
	{
		posIn.close();
		cout<<"Load POS Model "<<endl;
		knowledge->loadPOSModel(cate.data());
	}
	else
	{
		cout<<"Cannot found POS model, ignore POS output "<<endl;
		analyzer->setOption(Analyzer::OPTION_TYPE_POS_TAGGING, 0);
	}

    cout<<"Load User Dictionary "<<endl;
    knowledge->loadUserDict((cate+".dic").data());

    analyzer->setKnowledge(knowledge);
    analyzer->setPOSDelimiter(posDelimiter.data());

    cout<<"Start Tagging "<<endl;
    VTimer timer("Tag " + inFile);
    timer.start();
    analyzer->runWithStream(inFile.data(), outFile.data());
    timer.endAndPrint();
    
}

void printSeggerUsage(){
    cout<<"SYNOPSIS\n ./cmacsegger cateFile inFile outFile [encoding] [posDelimiter]"<<endl;
    cout<<"Description"<<endl;
    cout<<"   cateFile is the category file, there are several files are "<<
            "created after the training, and with cateFile as the prefix, "<<
            "prefix should contains both path and name, such /dir1/dir2/n1"<<endl;
    cout<<"   inFile is the input file"<<endl;
    cout<<"   outFile is the output file"<<endl;
    cout<<"   encoding is the encoding of the mateFile, and gb2312 "<<
            "is the default encoding. Only support gb2312 and big5 now"<<endl;
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
    string encoding = "gb2312";
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
