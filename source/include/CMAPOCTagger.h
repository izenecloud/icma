/* 
 * File:   CMAPOCTagger.h
 * Author: vernkin
 *
 * Created on March 24, 2009, 8:52 PM
 */

#ifndef _CMAPOCTAGGER_H
#define	_CMAPOCTAGGER_H

#include "cmacconfig.h"
#include "CMABasicTrainer.h"
#include "types.h"
#include "VTrie.h"

#include <algorithm>
#include <math.h>
#include <set>
#include <map>
using namespace maxent::me;

#define USE_POC_TRIE

namespace cma{

/**
 * POS context type for POC(Position of Character) (zh/chinese)
 */
void get_poc_zh_scontext(vector<string>& words, vector<string>& tags, size_t i,
        bool rareWord, vector<string>& context);

void poc_train(const char* file, const string& cateName,
        const char* extractFile = 0, string method = "gis", size_t iters = 15,
        float gaussian = 0.0f);

struct POCTagUnit{
    uint8_t pocCode;
    double score;
    int index;

    /** -1 if not exists*/
    int previous;
};

class SegTagger{
public:

    SegTagger(const string& cateName);

    void tag_file(const char* inFile, const char *outFile, 
            string encType = "gbk");

    /**
     * tagging given words list and return N best
     * \param words the given words list
     * \param N return N best
     * \param retSize retSize &lt;= N, the size of segment
     */
    void seg_sentence(vector<string>& words, size_t N, size_t retSize,
            vector<pair<vector<string>, double> >& segment);

    /**
     * only return the best segment result
     */
    void seg_sentence_best(vector<string>& words, vector<string>& segment);

    /**
     * Would be invoked by the SegTagger's Constructor
     */
    static void initialize();

    bool appendWordPOC(const string& line);

private:

    /**
     * tag word words[i] under given tag history hist
     * \param lastIndex the last index of candidates
     * \param canSize the used size in the candidates
     * \return a list of (tag, score) pair sorted
     */
    void tag_word(vector<string>& words, int index, size_t N, uint8_t* tags,
            POCTagUnit* candidates, int& lastIndex, size_t& canSize,
            double initScore, int candidateNum);

private:
    MaxentModel me;

    #ifdef USE_POC_TRIE
    /** Store the data to the POC tag (L/R/M/I) */
    VTrie trie_;
    #endif
};

}
#endif	/* _CMAPOCTAGGER_H */

