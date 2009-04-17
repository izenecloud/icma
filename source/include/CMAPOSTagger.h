/* 
 * File:   CMAPOSTagger.h
 * Author: vernkin
 *
 * Created on March 20, 2009, 10:46 AM
 */

#ifndef _CMAPOSTAGGER_H
#define	_CMAPOSTAGGER_H

#include "CMABasicTrainer.h"
#include "VTrie.h"
#include "strutil.h"
#include "CPPStringUtils.h"

#include <algorithm>
#include <math.h>
#include <vector>
#include <set>
#include <string>

using namespace maxent::me;

namespace cma{

inline bool isNumber(string& word){
    for(size_t i=0; i<word.length(); ++i){
        if(word[i] < '0' || word[i] > '9')
            return false;
    }
    return true;
}

inline bool isHyphen(string& word){
    return word.length() == 1 && word[0] == '-';
}

inline bool isUpperCase(string& word){
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
void get_pos_zh_scontext(vector<string>& words, vector<string>& tags, size_t i,
        bool rareWord, vector<string>& context);

inline bool cmpSDPair(pair<vector<string>,double> p1, pair<vector<string>,double> p2){
    return p1.second - p2.second >= 0;
}

void pos_train(const char* file, const string& cateFile,
        const char* extractFile = 0, string method = "gis", size_t iters = 15,
        float gaussian = 0.0f);

struct POSTagUnit{
    string pos;
    double score;
    int index;

    /** -1 if not exists*/
    int previous;
};

/**
 * Tagging the POS Information
 */
class POSTagger{
public:
    /**
     * Construct the POSTagger with outer VTrie
     */
    POSTagger(const string& model, VTrie* pTrie);

    /**
     * Construct the POSTagger with inner VTrie (read from dictFile)
     */
    POSTagger(const string& model, const char* dictFile);

    ~POSTagger();

    /**
     * Tag the segmented file with pos (In UTF8 Encoding)
     * \param inFile the input file
     * \param outFile the output file
     */
    void tag_file(const char* inFile, const char *outFile);

    /**
     * tagging given words list and return N best
     * \param words the given words list
     * \param N return N best
     * \param retSize retSize &lt;= N, the size of segment
     */
    void tag_sentence(vector<string>& words, size_t N, size_t retSize,
            vector<pair<vector<string>, double> >& segment);

    /**
     * Only return the best result
     */
    void tag_sentence_best(vector<string>& words, vector<string>& posRet);

    /**
     * Append the POS Information into Trie and POS Vector
     */
    bool appendWordPOS(string& line);

private:

    /**
     * tag word words[i] under given tag history hist
     * \param lastIndex the last index of candidates
     * \param canSize the used size in the candidates
     * \return a list of (tag, score) pair sorted
     */
    void tag_word(vector<string>& words, int index, size_t N, string* tags,
            POSTagUnit* candidates, int& lastIndex, size_t& canSize,
            double initScore, int candidateNum);

private:
    MaxentModel me;
    
    /** 
     * If the trie_ passed by parameter the trie_ is not supposed destroyed by
     * the POSTagger
     */
    VTrie *trie_;

    bool isInnerTrie_;

    vector<set<string> > posVec_;
};

}
#endif	/* _CMAPOSTAGGER_H */

