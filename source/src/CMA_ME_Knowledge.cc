/**
 * \author vernkin
 */

#include "CMA_ME_Knowledge.h"
#include "CPPStringUtils.h"
#include "strutil.h"

#include <assert.h>
#include <iostream>
#include <fstream>
using namespace std;

namespace cma{

CMA_ME_Knowledge::CMA_ME_Knowledge(): segT_(0), posT_(0),vsynC_(0){

}

CMA_ME_Knowledge::~CMA_ME_Knowledge(){
    delete segT_;
    delete posT_;
    delete vsynC_;
}

int CMA_ME_Knowledge::loadPOSModel(const char* cateName){
    assert(!posT_);
    string cateStr(cateName);
    posT_ = new POSTagger((cateStr + ".model").data(),
            (cateStr + ".tag").data());
    return 1;
}

int CMA_ME_Knowledge::loadStatModel(const char* cateName){
    assert(!segT_);
    string cateStr(cateName);
    segT_ = new SegTagger((cateStr + ".model").data(),
            (cateStr + ".tag").data());
    return 1;
}

int CMA_ME_Knowledge::loadSynonymDictionary(const char* fileName){
    assert(!vsynC_);
    vsynC_ = new VSynonymContainer(fileName, " ", "_");
}

void CMA_ME_Knowledge::getSynonyms(const string& word, VSynonym& synonym){
    vsynC_->get_synonyms(word, synonym);
}

int CMA_ME_Knowledge::loadStopWordDict(const char* fileName){
    ifstream in(fileName);
    string line;
    while(!in.eof()){
        getline(in, line);
        trimSelf(line);
        if(line.length() <= 0)
            continue;
        stopWords_.insert(F_UTF8W(line));
    }
    in.close();
    return 1;
}

int CMA_ME_Knowledge::loadSystemDict(const char* binFileName){
    ifstream in(binFileName);
    string line;
    while(!in.eof()){
        //may be another way get line
        getline(in, line);
        
        
    }
    in.close();
    return 1;
}

int CMA_ME_Knowledge::loadUserDict(const char* fileName){
    return 1;
}

int CMA_ME_Knowledge::encodeSystemDict(const char* txtFileName,
        const char* binFileName){
    return 1;
}

SegTagger* CMA_ME_Knowledge::getSegTagger() const{
    return segT_;
}

POSTagger* CMA_ME_Knowledge::getPOSTagger() const{
    return posT_;
}

string CMA_ME_Knowledge::decodeEncryptWord(const string& origWord){
    return origWord;
}

bool CMA_ME_Knowledge::isStopWord(const wstring& word){
    return stopWords_.find(word) != stopWords_.end();
}

}
