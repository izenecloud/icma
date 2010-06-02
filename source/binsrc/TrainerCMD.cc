#define TRAINERCMD_AS_MAIN

#include "TrainerCMD.h"

void beginTrain(string pocXmlFile, string pMateFile, string cateFile, string enc,
        string posDelimiter, bool trainPOS = true, bool trainPOC = true,
        bool largeCorpus = false){
    string mateFile = pMateFile;

    Knowledge::EncodeType encType = Knowledge::decodeEncodeType(enc.c_str());

    if( encType == Knowledge::ENCODE_TYPE_NUM )
    {
    	cerr<<"[Error] Can't find encoding "<<enc<<"."<<endl;
    	exit(1);
    }

#ifdef USE_UTF_16
    if(encType == Knowledge::ENCODE_TYPE_UTF16)
    {
    	const unsigned char* uc = (const unsigned char*)posDelimiter.c_str();
    	if(uc[0] < 0x80)
    	{
    		posDelimiter = "\0" + posDelimiter;
    	}
    }
#endif

    int loadXml = CMA_CType::instance(encType)->loadConfiguration(pocXmlFile.data());
    if( !loadXml )
	{
		cerr<<" Fail to load "<<pocXmlFile<<", please check that file! "<<endl;
		exit(1);
		return;
	}

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

		cout<<"#Start Training POC Model..."<<endl;
		poc_train(matePOC.data(), cateFile, encType, posDelimiter, largeCorpus);

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
    cout<<"SYNOPSIS\n     ./cmactrainer pocXmlFile mateFile modelPath [encoding] [posDelimiter] "<<
    		"[pos | poc | pos+poc] [large-corpus]"<<endl;
    cout<<"Description"<<endl;
    cout<<"   pocXmlFile is the path of Segmentation XML file in the associated encoding. One sample is "<<
            "db/icwb/utf8/poc.xml (With utf8 encoding)."<<endl;
    cout<<"   mateFile is the material file, the words separated by spaces and maybe "
    		"has POS information. For example, mateFile with POS information: word1/pos1 word2/pos2...; "<<
    		"mateFile without POS information: word1 word2...."<<endl;
    cout<<"   modelPath is the path of the directory what will contains all the model. Like "<<
            "poc.model, sys.dic and so on."<<endl;
    cout<<"   encoding is the encoding of the mateFile, Default value is gb18030. "<<
            "Support utf8, gb2312, gb18030 and big5 now. Recommend to use utf8/gb18030 to support "<<
            "both Simplified and Traditional Chinese."<<endl;
    cout<<"   posDelimiter is the delimiter between the word and the pos tag. Default value is '/'."<<
            "Like '/' and '_'. If the taining model is poc(the next parameter) and the mateFile doesn't "<<
            "contains POS information, posDemiliter can be any single character (recommend to ues '/')."<<endl;
    cout<<"   pos | poc | pos+poc is the training mode. Default value is pos+poc. poc only trains "<<
            "segmentation model, pos only trains POS tagging model, and pos+poc trains both of them."<<endl;
    cout<<"   large-corpus indicates whether the mateFile is large ( > 20MB). Default value in none "<<
            "(don't add parameter large-corpus). some parameters are adjusted for large mateFile."<<endl;
    cout<<"Notice: Configuration files ( cma.config, poc.xml and pos.config) are not created automatically. You can copy these "<<
          "files with same names under db/icwb/utf8 (If the encoding of mateFile isn't utf8, require to change the encoding of "<<
          "copied files)."<<endl;
    cout<<endl;
    cout<<"For example ( create the directory modelPath first and put the poc.xml under modelPath ): "<<endl;
    cout<<"   1) To train a larger utf8 file for both segmentation for POS tagging with '/' as POS separator, use:"<<endl;
    cout<<"      ./cmactrainer modelPath/poc.xml mateFile modelPath utf8 / pos+poc large-corpus"<<endl;
    cout<<"   2) To train a small utf8 file just for segmentation with '/' as POS separator (It is OK "<<
          "if don't contain POS information, such as: word1 word2 word3. And just use '/' as POS separator ):"<<endl;
    cout<<"      ./cmactrainer modelPath/poc.xml mateFile modelPath utf8 / poc"<<endl;
}

int tainerEntry(int argc, char** argv){
    if(argc < 3){
        printTainerUsage();
        return 0;
    }
    string pocXmlFile(argv[1]);
    string pMateFile(argv[2]);
    string cateFile(argv[3]);
    string encoding = "gb18030";
    if(argc > 4){
        encoding = argv[4];
    }
    string posDelimiter = "/";
    if(argc > 5){
        posDelimiter = argv[5];
    }
    bool trainPOS = true;
    bool trainPOC = true;
    if(argc > 6){
    	if(!strcmp(argv[6],"pos"))
        	trainPOC = false;
        else if(!strcmp(argv[6],"poc"))
        	trainPOS = false;
        else if(strcmp(argv[6],"pos+poc")){
        	cout<<"[Error] Can't find parameter 6: should be pos | poc | pos+poc, "<<
        			"your input is "<<argv[6]<<"."<<endl;
        	printTainerUsage();
        	return 0;
        }
    }

    bool largeCorpus = false;
    if(argc > 7){
    	largeCorpus = !strcmp(argv[7],"large-corpus");
    }

    beginTrain(pocXmlFile, pMateFile, cateFile, encoding, posDelimiter, trainPOS, trainPOC, largeCorpus);
    return 1;
}

#ifdef TRAINERCMD_AS_MAIN
int main(int argc, char** argv){
    return tainerEntry(argc, argv);
}
#endif
