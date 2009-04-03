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

#include <algorithm>
#include <math.h>
#include <set>
#include <map>
using namespace maxent::me;

namespace cma{

/**
 * POS context type for POC(Position of Character) (zh/chinese)
 */
void get_poc_zh_scontext(vector<string>& words, vector<string>& tags, size_t i,
        bool rareWord, vector<string>& context);

void poc_train(const char* file, const string& cateName,
        const char* extractFile = 0, string method = "gis", size_t iters = 15,
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

struct POCTagUnit{
    uint8_t pocCode;
    double score;
    int index;

    /** -1 if not exists*/
    int previous;
};

class SegTagger{
public:

    SegTagger(const string& model, const char* tagDictFile){
        me.load(model);
    }

    void tag_file(const char* inFile, const char *outFile);

    /**
     * tagging given words list and return N best
     * \param words the given words list
     * \param N return N best
     * \param retSize retSize &lt;= N, the size of segment
     */
    void seg_sentence(vector<string>& words, size_t N, size_t retSize,
            vector<pair<vector<string>, double> >& segment);

    static void initialize();

private:

    /**
     * tag word words[i] under given tag history hist
     * \param lastIndex the last index of candidates
     * \param canSize the used size in the candidates
     * \return a list of (tag, score) pair sorted
     */
    void tag_word(vector<string>& words, int index, size_t N, uint8_t* tags,
            POCTagUnit* candidates, int& lastIndex, int& canSize, 
            double initScore, int candidateNum);

private:
    MaxentModel me;
};

}
#endif	/* _CMAPOCTAGGER_H */

