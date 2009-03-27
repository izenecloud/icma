#include "CMA_ME_Analyzer.h"

#include <fstream>
#include <iostream>

#include "strutil.h"

namespace cma
{
    
CMA_ME_Analyzer::CMA_ME_Analyzer():knowledge_(0){

}

CMA_ME_Analyzer::~Analyzer(){
    delete knowledge_;
}

int CMA_ME_Analyzer::runWithSentence(Sentence& sentence){
    int N = (int)options_[N_BEST];
    bool printPOS = options_[OPTION_TYPE_POS_TAGGING] > 0;
    
    return -1;
}

int CMA_ME_Analyzer::runWithStream(const char* inFileName, const char* outFileName){
    int N = 1;
    bool printPOS = options_[OPTION_TYPE_POS_TAGGING] > 0;

    ifstream in(inFileName);
    ofstream out(outFileName);

    string line;
    bool remains = !in.eof();
    while(remains){
        getline(in, line);
        remains = !in.eof();
        trimSelf(line);
        if(!line.length()){
            out<<endl;
            continue;
        }
        vector<pair<vector<string>,double> > segment;
        vector<vector<string> > pos;
        analysis(line, N, pos, segment, printPOS);

        if(printPOS){
            vector<string>& best = segment[0].first;
            vector<string>& bestPOS = pos[0];
            size_t n = best.size();
            for(size_t i=0; i<n; ++n){
                out<<best[i]<<"/"<<bestPOS[i];
                if(i < n-1){
                    out<<" ";
                }else if(remains){
                    out<<endl;
                }else{
                    break;
                }
            }
        }else{
            vector<wstring>& best = segment[0].first;
            size_t n = best.size();
            for(size_t i=0; i<n; ++n){
                out<<best[i];
                if(i < n-1){
                    out<<" ";
                }else if(remains){
                    out<<endl;
                }else{
                    break;
                }
            }
        }


    }
    in.close();
    out.close();
    return 1;
}

const char* CMA_ME_Analyzer::runWithString(const char* inStr){
    int N = (int)options_[N_BEST];
    return 0;
}

void CMA_ME_Analyzer::setKnowledge(Knowledge* pKnowledge){
    knowledge_ = (CMA_ME_Knowledge*)pKnowledge;
}

void CMA_ME_Analyzer::analysis(const string& sentence, int N,
        vector<vector<string> >& pos,
        vector<pair<vector<string>,double> >& segment, bool tagPOS){
    int i = 0;

}

}
