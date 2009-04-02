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

CMA_ME_Knowledge::CMA_ME_Knowledge(): segT_(0), posT_(0),vsynC_(0),trie_(0){

}

CMA_ME_Knowledge::~CMA_ME_Knowledge(){
    delete segT_;
    delete posT_;
    delete vsynC_;
    delete trie_;
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
        stopWords_.insert(line);
    }
    in.close();
    return 1;
}

int CMA_ME_Knowledge::loadSystemDict(const char* binFileName){
    assert(posT_);
    if(!trie_)
        trie_ = new VTrie();

    ifstream in(binFileName);
    string line;
    while(!in.eof()){
        //may be another way get line
        getline(in, line);
        loadOuterDictRecord(line, 5);
    }
    in.close();
    return 1;
}

int CMA_ME_Knowledge::loadUserDict(const char* fileName){
    assert(posT_);
    if(!trie_)
        trie_ = new VTrie();

    ifstream in(fileName);
    string line;
    while(!in.eof()){
        getline(in, line);
        loadOuterDictRecord(line, 10);
    }
    in.close();
    return 1;
}

void CMA_ME_Knowledge::loadOuterDictRecord(const string& record, int counter){
    vector<string> tokens;
    TOKEN_STR(record, tokens);
    size_t n = tokens.size();
    if(!n)
        return;
    string word = tokens[0];
    replaceAll(word, "_", " ");

    VTrieNode node;
    trie_->insert(word.data(), &node);
    for(size_t i=1; i<n; ++i){
        posT_->appendWordPOS(word, tokens[i], counter);
    }
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

bool CMA_ME_Knowledge::isStopWord(const string& word){
    return stopWords_.find(word) != stopWords_.end();
}

VTrie* CMA_ME_Knowledge::getTrie(){
    return trie_;
}

}
