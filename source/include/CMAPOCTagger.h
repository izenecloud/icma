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

namespace cma{

/**
 * POS context type for POC(Position of Character) (zh/chinese)
 */
void get_poc_zh_scontext(vector<string>& words, vector<string>& tags, size_t i,
        bool rareWord, vector<string>& context, CMA_CType *ctype);

void poc_train(const char* file, const string& cateName,
        Knowledge::EncodeType encType = Knowledge::ENCODE_TYPE_GB2312,
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

    /**
     * Create the SegTagger
     * \param cateName the poc category name, the model file(cateName + ".model")
     *     should exists.
     * \param posTrie the VTrie to hold the POS Information
     * \param eScore is a double value between 0.5 and 1.0, if the POC tag B has
     * possiblity more the eScore, it will be tagged with E. EScore's default
     * value is 0.7.
     */
    SegTagger(const string& cateName, VTrie* posTrie, double eScore = 0.7);

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
     * only return the best segment result, no scores is used here
     * \param words the word list
     * \param segment to store the segmented words
     */
    void seg_sentence_best(vector<string>& words, vector<string>& segment);

    /**
     * Would be invoked by the SegTagger's Constructor
     */
    static void initialize();

    void setCType(CMA_CType *ctype){
        ctype_ = ctype;
    }

    /**
     * Set the EScore<BR>
     * EScore is a double value between 0.5 and 1.0, if the POC tag B has
     * possiblity more the eScore, it will be tagged with E. <BR>
     * EScore's default value is 0.7.
     *
     * \return eScore the new eScore
     */
    void setEScore(double eScore){
        assert( eScore > 0.5 && eScore <= 1.0 && "eScore should between 0.5 and 1.0");
        eScore_ = eScore;
    }

    /**
     * Get the EScore <BR>
     * EScore is a double value between 0.5 and 1.0, if the POC tag B has
     * possiblity more the eScore, it will be tagged with E. <BR>
     * EScore's default value is 0.7.
     *
     * \return the EScore
     */
    double getEScore(){
        return eScore_;
    }

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

    CMA_CType *ctype_;

    /** Store the data to the POS tags */
    VTrie* trie_;

    double eScore_;
};

}
#endif	/* _CMAPOCTAGGER_H */

