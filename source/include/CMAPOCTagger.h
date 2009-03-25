/* 
 * File:   CMAPOCTagger.h
 * Author: vernkin
 *
 * Created on March 24, 2009, 8:52 PM
 */

#ifndef _CMAPOCTAGGER_H
#define	_CMAPOCTAGGER_H

#include "CMABasicTrainer.h"

#include <algorithm>
#include <math.h>

using namespace maxent::me;

namespace cma{

/**
 * POS context type for POC(Position of Character) (zh/chinese)
 */
void get_poc_zh_scontext(vector<wstring>& words, vector<wstring>& tags, size_t i,
        bool rareWord, vector<wstring>& context);

inline bool cmpSDPOCPair(pair<vector<wstring>,double> p1, pair<vector<wstring>,double> p2){
    return p1.second - p2.second >= 0;
}

void poc_train(const char* file, const string& destFile,
        const char* extractFile = 0, string method = "lbfgs", size_t iters = 15,
        float gaussian = 0.0f);

/**
 * The input is AB/tag2 CDF/tag2 G/tag3 ... <br>
 * The output is A/L B/R C/L D/M F/M G/I <br>
 * L is Left; R is Rightl M is Middle; I is independent.
 *
 * \param inFile the input file
 * \param outFile the output file
 */
void create_poc_meterial(const char* inFile, const char* outFile);

inline bool is_matched_poc(wstring& preTag, wstring& curTag){
    if(preTag == L"R" || preTag == L"I")
        return curTag == L"L" || curTag == L"I";
        
    if(preTag == L"L")
            return curTag == L"R" || curTag == L"M";
        
    if(preTag == L"M")
            return curTag == L"R" || curTag == L"M";
     
    cerr<<"Invalid POC tag "<<CPPStringUtils::to_utf8(preTag)<<endl;
    exit(1);
    return false;
}

class SegTagger{
public:
    SegTagger(const string& model, const char* tagDictFile,
             context_t context = get_poc_zh_scontext){
        me.load(model);
        load_tag_dict(&tagDict_, tagDictFile);
        get_context = context;
    }

    void tag_file(const char* inFile, const char *outFile);

    /**
     * tagging given sentence s and return N best
     * \param senetence the given sentence
     * \param N return N best
     */
    void seg_sentence(wstring sentence, size_t N,
            vector<pair<vector<wstring>, double> >& segment);

private:

    /**
     * tag word words[i] under given tag history hist
     *
     * \return a list of (tag, score) pair sorted
     */
    void tag_word(vector<wstring>& words, int i, size_t N, vector<wstring>& hist,
        vector<pair<wstring, double> >& ret);

    void advance(pair<vector<wstring>, double> tag, vector<wstring>& words,
            int i, size_t N, vector<pair<vector<wstring>, double> >& ret);

private:
    MaxentModel me;
    map<wstring, map<wstring, int> > tagDict_;
    context_t get_context;
};

}
#endif	/* _CMAPOCTAGGER_H */

