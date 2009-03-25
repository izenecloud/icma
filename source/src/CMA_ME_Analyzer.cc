#include "CMA_ME_Analyzer.h"

namespace cma
{
    
CMA_ME_Analyzer::CMA_ME_Analyzer(){

}

int CMA_ME_Analyzer::runWithSentence(Sentence& sentence){
    return -1;
}

int CMA_ME_Analyzer::runWithStream(const char* inFileName, const char* outFileName){
    return -1;
}

const char* CMA_ME_Analyzer::runWithString(const char* inStr){
    return 0;
}

void CMA_ME_Analyzer::setKnowledge(Knowledge* pKnowledge){
    knowledge_ = (CMA_ME_Knowledge*)pKnowledge;
}

}
