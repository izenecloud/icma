#include "CMA_ME_Analyzer.h"

#include <fstream>
#include <iostream>
#include <bits/stl_vector.h>

#include "strutil.h"
#include "sentence.h"

namespace cma
{
    
CMA_ME_Analyzer::CMA_ME_Analyzer():knowledge_(0){
    SegTagger::initialize();
}

CMA_ME_Analyzer::~CMA_ME_Analyzer(){
    delete knowledge_;
}

int CMA_ME_Analyzer::runWithSentence(Sentence& sentence){
    int N = (int)getOption(N_BEST);

    vector<pair<vector<string>,double> > segment;
    vector<vector<string> > pos;
    analysis(string(sentence.getString()), N, pos, segment, true);

    size_t size = segment.size();
    for(size_t i=0; i<size; ++i){
        MorphemeList list;
        vector<string>& segs = segment[i].first;
        vector<string>& poses = pos[i];
        size_t m = segs.size();
        for(size_t j=0; j<m; ++j){
            Morpheme morp;
            morp.lexicon_ = segs[j];
            morp.posCode_ = Sentence::getPOSCode(poses[j]);
            list.push_back(morp);
        }
        sentence.addList(list, segment[i].second);
    }
    
    return 1;
}

int CMA_ME_Analyzer::runWithStream(const char* inFileName, const char* outFileName){
    int N = 1;
    bool printPOS = getOption(OPTION_TYPE_POS_TAGGING) > 0;

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
            vector<string>& best = segment[0].first;
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
    bool printPOS = getOption(OPTION_TYPE_POS_TAGGING) > 0;

    string line(inStr);

    vector<pair<vector<string>,double> > segment;
    vector<vector<string> > pos;
    analysis(line, 1, pos, segment, printPOS);

    strBuf_.clear();
    if(printPOS){
        vector<string>& best = segment[0].first;
        vector<string>& bestPOS = pos[0];
        size_t n = best.size();
        for(size_t i=0; i<n; ++n){
            strBuf_.append(best[i]).append("/").append(bestPOS[i]);
            if(i < n-1){
                strBuf_.append(" ");
            }else{
                break;
            }
        }
    }else{
        vector<string>& best = segment[0].first;
        size_t n = best.size();
        for(size_t i=0; i<n; ++n){
            strBuf_.append(best[i]);
            if(i < n-1){
                strBuf_.append(" ");
            }else{
                break;
            }
        }
    }

    return strBuf_.c_str();
}

void CMA_ME_Analyzer::setKnowledge(Knowledge* pKnowledge){
    knowledge_ = (CMA_ME_Knowledge*)pKnowledge;
}

void CMA_ME_Analyzer::analysis(const string& sentence, int N,
        vector<vector<string> >& pos,
        vector<pair<vector<string>,double> >& segment, bool tagPOS){
    int minN = N > 1 ? N : 2;

}

}
