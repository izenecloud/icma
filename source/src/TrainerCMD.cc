
#include "cma_ctype.h"

#include "CMAPOCTagger.h"
#include "CMAPOSTagger.h"
#include <stdlib.h>

using namespace cma;

void beginTrain(string pMateFile, string cateFile, string enc = "gbk"){
    string mateFile = pMateFile;

    Knowledge::EncodeType encType = CMA_CType::getEncType(enc);

    cout<<"#Start Training POS Model..."<<endl;
    pos_train(mateFile.data(), cateFile, encType);

    cout<<"#Generate POC Material File "<<endl;
    string matePOC = mateFile + ".poc.tmp";
    create_poc_meterial(mateFile.data(), matePOC.data(), encType);

    string cateFilePOC = cateFile + "-poc";
    cout<<"#Start Training POC Model..."<<endl;
    poc_train(matePOC.data(), cateFilePOC, encType);

    //remove matePOC and mateFile(if tmpMateFile is true)
    cout<<"#Remove temporal files "<<endl;
    remove(matePOC.data());

    cout<<"#Training Finished!"<<endl;
}

void printTainerUsage(){
    cout<<"The Format is: ./tainercmd mateFile cateFile [encoding]"<<endl;
    cout<<"   mateFile is the material file, it should be in the form "<<
            "word1/pos1 word2/pos2 word3/pos3 ..."<<endl;
    cout<<"   cateFile is the category file, there are several files are "<<
            "created after the trainging, and with cateFile as the prefix, "<<
            "prefix should contains both path and name, such /dir1/dir2/n1"<<endl;
    cout<<"   encoding (optional) is the encoding of the mateFile, and gbk "<<
            "is the default encoding"<<endl;
}

int tainerEntry(int argc, char** argv){
    if(argc < 3){
        printTainerUsage();
        return 0;
    }
    string pMateFile(argv[1]);
    string cateFile(argv[2]);
    string encoding;
    if(argc > 3){
        encoding = argv[3];
    }
    beginTrain(pMateFile, cateFile, encoding);
    return 1;
}

int main(int argc, char** argv){
    return tainerEntry(argc, argv);
}
