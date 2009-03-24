/* 
 * File:   CMAPOSTagger.h
 * Author: vernkin
 *
 * Created on March 20, 2009, 10:46 AM
 */

#ifndef _CMAPOSTAGGER_H
#define	_CMAPOSTAGGER_H

#include "CMABasicTrainer.h"

#include <algorithm>
#include <math.h>

using namespace maxent::me;

namespace cma{

inline bool isNumber(wstring& word){
    for(size_t i=0; i<word.length(); ++i){
        if(word[i] < '0' || word[i] > '9')
            return false;
    }
    return true;
}

inline bool isHyphen(wstring& word){
    return word.length() == 1 && word[0] == '-';
}

inline bool isUpperCase(wstring& word){
    for(size_t i=0; i<word.length(); ++i){
        if(word[i] < 'A' || word[i] > 'Z')
            return false;
    }
    return true;
}

void get_prefix_suffix(wstring& word, size_t length, vector<wstring>& prefixes,
        vector<wstring>& suffixes);

/**
 * POS context type for POS (zh/chinese)
 */
void get_pos_zh_scontext(vector<wstring>& words, vector<wstring>& tags, size_t i,
        bool rareWord, vector<wstring>& context);

inline bool cmpSDPair(pair<vector<wstring>,double> p1, pair<vector<wstring>,double> p2){
    return p1.second - p2.second >= 0;
}

void pos_train(const char* file, const string& destFile,
        const char* extractFile = 0, string method = "lbfgs", size_t iters = 15,
        float gaussian = 0.0f);


class POSTagger{
public:
    POSTagger(const string& model, const char* tagDictFile,
             context_t context = get_pos_zh_scontext){
        me.load(model);
        load_tag_dict(&tagDict_, tagDictFile);
        get_context = context;
    }

    void tag_file(const char* inFile, const char *outFile);

    /**
     * tagging given sentence s and return N best
     * \param words a list of words to tag
     * \param N return N best
     */
    void tag_sentence(vector<wstring>& words, size_t N,
            vector<pair<vector<wstring>, double> >& h0);

private:

    /**
     * tag word words[i] under given tag history hist
     *
     * \return a list of (tag, score) pair sorted
     */
    void tag_word(vector<wstring>& words, int i, vector<wstring>& hist,
        vector<pair<wstring, double> >& ret);

    void advance(pair<vector<wstring>, double> tag, vector<wstring>& words,
            int i, size_t N, vector<pair<vector<wstring>, double> >& ret);

private:
    MaxentModel me;
    map<wstring, map<wstring, int> > tagDict_;
    context_t get_context;
};

}
#endif	/* _CMAPOSTAGGER_H */

