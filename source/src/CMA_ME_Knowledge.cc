/**
 * \author vernkin
 */

#include "CMA_ME_Knowledge.h"
#include <assert.h>

namespace cma{

CMA_ME_Knowledge::CMA_ME_Knowledge(): segT_(0), posT_(0){

}

CMA_ME_Knowledge::~CMA_ME_Knowledge(){
    delete segT_;
    delete posT_;
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

int CMA_ME_Knowledge::loadStopWordDict(const char* fileName){
    return 1;
}

int CMA_ME_Knowledge::loadSystemDict(const char* binFileName){
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

}
