#define TRAINERCMD_AS_MAIN

#include "TrainerCMD.h"

void beginTrain(string pMateFile, string cateFile, string enc, 
        string posDelimiter, bool trainPOS = true, bool trainPOC = true,
        bool largeCorpus = false){
    string mateFile = pMateFile;

    Knowledge::EncodeType encType = Knowledge::decodeEncodeType(enc.c_str());

    if(trainPOS)
    {
        cout<<"#Start Training POS Model..."<<endl;
        pos_train(mateFile.data(), cateFile, encType, posDelimiter, largeCorpus);
    }
    else
    {
    	cout<<"#[Warning] User Cancel Training POS Model"<<endl;
    }

    if(trainPOC)
    {
		cout<<"#Generate POC Material File "<<endl;
		string matePOC = mateFile + ".poc.tmp";
		create_poc_meterial(mateFile.data(), matePOC.data(), encType, posDelimiter);

		string cateFilePOC = cateFile + "-poc";
		cout<<"#Start Training POC Model..."<<endl;
		poc_train(matePOC.data(), cateFilePOC, encType, posDelimiter, largeCorpus);

		//remove matePOC and mateFile(if tmpMateFile is true)
		cout<<"#Remove temporal files "<<endl;
		remove(matePOC.data());
    }
    else
	{
		cout<<"#[Warning] User Cancel Training POC Model"<<endl;
	}
    cout<<"#Training Finished!"<<endl;
}

void printTainerUsage(){
    cout<<"SYNOPSIS\n     ./cmactrainer modelPath cateFile [encoding] [posDelimiter] "<<
    		"[pos | poc | pos+poc] [large-corpus]"<<endl;
    cout<<"Description"<<endl;
    cout<<"   modelPath is the path of the directory what will contains all the model, like "<<
            "poc.model, sys.dic and so on."<<endl;
    cout<<"   cateFile is the category file, there are several files are "<<
            "created after the training, and with cateFile as the prefix, "<<
            "prefix should contains both path and name, such /dir1/dir2/n1"<<endl;
    cout<<"   encoding is the encoding of the mateFile, and gb18030 "<<
            "is the default encoding. Only support gb2312, gb18030 and big5 now"<<endl;
    cout<<"   posDelimiter is the delimiter between the word and the pos tag, "<<
            "like '/' and '_' and default is '/'"<<endl;
    cout<<"   pos | poc | pos+poc indicate only train POS | POC |POS and POC models"<<endl;
    cout<<"   large-corpus indicates the corpus is large, some parameters are set larger"<<endl;
}

int tainerEntry(int argc, char** argv){
    if(argc < 3){
        printTainerUsage();
        return 0;
    }
    string pMateFile(argv[1]);
    string cateFile(argv[2]);
    string encoding = "gb18030";
    if(argc > 3){
        encoding = argv[3];
    }
    string posDelimiter = "/";
    if(argc > 4){
        posDelimiter = argv[4];
    }
    bool trainPOS = true;
    bool trainPOC = true;
    if(argc > 5){
    	if(!strcmp(argv[5],"pos"))
        	trainPOC = false;
        else if(!strcmp(argv[5],"poc"))
        	trainPOS = false;
        else if(strcmp(argv[5],"pos+poc")){
        	cout<<"[Error] Can't find parameter 5: should be pos | poc | pos+poc, "<<
        			"your input is "<<argv[5]<<"."<<endl;
        	printTainerUsage();
        	return 0;
        }
    }

    bool largeCorpus = false;
    if(argc > 6){
    	largeCorpus = !strcmp(argv[6],"large-corpus");
    }

    beginTrain(pMateFile, cateFile, encoding, posDelimiter, trainPOS, trainPOC, largeCorpus);
    return 1;
}

#ifdef TRAINERCMD_AS_MAIN
int main(int argc, char** argv){
    return tainerEntry(argc, argv);
}
#endif
