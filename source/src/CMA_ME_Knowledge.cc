/**
 * \author vernkin
 */

#include "CMA_ME_Knowledge.h"
#include "CPPStringUtils.h"
#include "strutil.h"

#include <assert.h>
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

    FILE *in = fopen(binFileName, "r");
    string line;
    while(!feof(in)){
        //may be another way get line
        line = readEncryptLine(in);
        if(line.empty())
            break;

        loadOuterDictRecord(line, 5);
    }
    fclose(in);
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
    ifstream in(txtFileName);
    FILE *out = fopen(binFileName, "w");
    string line;

    int seCode[] = {0x12, 0x34, 0x54, 0x27};

    while(!in.eof()){
        getline(in, line);
        int size = (int)line.size();
        if(!size)
            continue;
        char buf[size + 1];
        buf[size] = '\0';
        strncpy(buf, line.data(), size);    

        for(int i=0; i<size; ++i){
            if( i % 2 == 0)
                buf[i] += 2;
            else if( i % 3 == 0)
                buf[i] += 3;
            else if( i % 5 == 0)
                buf[i] += 5;
            else if( i % 7 == 0)
                buf[i] += 7;
            buf[i] ^= seCode[ i % 4 ];
        }

        for(int i=0; i<=size/2; i+=2){
            char tmp = buf[i];
            buf[i] = buf[ size - 1 - i ];
            buf[ size - 1 - i ] = tmp;
        }

        //swap the length bit
        fputc(size >> 8 & 0xff, out);
        fputc(size >> 24 & 0xff, out);
        fputc(size & 0xff, out);
        fputc(size >> 16 & 0xff, out);
        fputs(buf, out);
    }

    in.close();
    fclose(out);
    return 1;
}

SegTagger* CMA_ME_Knowledge::getSegTagger() const{
    return segT_;
}

POSTagger* CMA_ME_Knowledge::getPOSTagger() const{
    return posT_;
}

string CMA_ME_Knowledge::readEncryptLine(FILE *in){
    int seCode[] = {0x12, 0x34, 0x54, 0x27};
    int lenBuf[4];
    lenBuf[0] = fgetc(in);
    if(lenBuf[0] == -1)
        return "";
    for(int i=1; i<4; ++i){
        lenBuf[i] = fgetc(in);
    }
    int size = (lenBuf[0] << 8) + (lenBuf[1] << 24) + (lenBuf[2]) + (lenBuf[3] << 16);

    char buf[size + 1];
    buf[size] = '\0';
    fgets(buf, size + 1, in);

    for(int i=0; i<=size/2; i+=2){
        char tmp = buf[i];
        buf[i] = buf[ size - 1 - i ];
        buf[ size - 1 - i ] = tmp;
    }

    for(int i=0; i<size; ++i){
        buf[i] ^= seCode[ i % 4 ];
        if( i % 2 == 0)
            buf[i] -= 2;
        else if( i % 3 == 0)
            buf[i] -= 3;
        else if( i % 5 == 0)
            buf[i] -= 5;
        else if( i % 7 == 0)
            buf[i] -= 7;
    }

    return string(buf);
}

bool CMA_ME_Knowledge::isStopWord(const string& word){
    return stopWords_.find(word) != stopWords_.end();
}

VTrie* CMA_ME_Knowledge::getTrie(){
    return trie_;
}

}
