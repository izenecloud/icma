/** \file CMAPOCTagger.h
 * \brief for the POC tagger using MaxEnt Model.
 *
 * \author vernkin
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

/** max word length that combined by probability but not dictionary */
#define MAX_PROP_WORD_LEN 2

/** min word length found in the pre-processing */
#define MIN_PRE_WORD_LEN 4

namespace cma{

/**
 * Get context of POC(Position of Character) (zh/chinese)
 *
 * \param words the words vector
 * \param tags the poc tags vector
 * \param i the index in the wrods
 * \param rareWord whether this word is rareWord
 * \param context to hold the return context
 * \param ctype indicates the encoding types
 */
void get_poc_zh_scontext(vector<string>& words, vector<string>& tags, size_t i,
        bool rareWord, vector<string>& context, CMA_CType *ctype);

/**
 * Training the POC Maxent model
 *
 * \param file the source file, with formate "word1/tag1 word2/tag2 ..."
 * \param cateFile the cateFile (include the path) is the prefix of all the file
 *    that created while training
 * \param encType the encoding type, default is gb18030
 * \param extractFile if set, save the training data to the extractFile and exit
 * \param iters how many iterations are required for training[default=15]
 * \param method the method of Maximum Model Parameters Estimation [default = gis]
 * \param gaussian apply Gaussian penality when training [default=0.0]
 * \param isPOS if true, output the tag dictioanry
 */
void poc_train(const char* file, const string& cateName,
        Knowledge::EncodeType encType = Knowledge::ENCODE_TYPE_GB18030,
        string pocDelimiter = "/", bool isLargeCorpus = false,
        const char* extractFile = 0, string method = "gis", size_t iters = 15,
        float gaussian = 0.0f);

/**
 * \brief to hold the objects in the N-best algorithm
 *
 */
struct POCTagUnit{
    /**
     * the poc code (B/E)
     */
    uint8_t pocCode;
    
    /**
     * the score for the current poc
     */
    double score;

    /**
     * the candidate number
     */
    int index;

    /** 
     * Previous index in the candidate list. -1 if not exists
     */
    int previous;
};

/**
 * \brief segment the string
 * segment the string using maxent model
 */
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

    ~SegTagger();

    void tag_file(const char* inFile, const char *outFile, 
            string encType = "gb2312");

    /**
     * tagging given words list and return N best
     * \param words the given words list
     * \param N return N best
     * \param retSize retSize &lt;= N, the size of segment
     */
    void seg_sentence(vector<string>& words, CharType *types,
            size_t N, size_t retSize,
            vector<pair<vector<string>, double> >& segment);

    /**
     * only return the best segment result, no scores is used here
     * \param words the word list
     * \param segment to store the segmented words
     */
    void seg_sentence_best(vector<string>& words, CharType *types,
            vector<string>& segment);

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
    void tag_word(vector<string>& words, CharType *type, int index, size_t N,
            uint8_t* tags, POCTagUnit* candidates, int& lastIndex, size_t& canSize,
            double initScore, int candidateNum);

    /** Find out the words contains at least 4 characters */
    void preProcess(vector<string>& words, uint8_t* tags);

private:
    /** The MaxEnt model Object */
    MaxentModel me;

    /** The encoding type */
    CMA_CType *ctype_;

    /** Store the data to the POS tags */
    VTrie* trie_;

    /**
     * EScore is a double value between 0.5 and 1.0, if the POC tag B has
     * possiblity more the eScore, it will be tagged with E. <BR>
     */
    double eScore_;
};

}
#endif	/* _CMAPOCTAGGER_H */

