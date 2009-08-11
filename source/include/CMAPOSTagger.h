/** \file CMAPOSTagger.h
 * \brief for the POS tagger using MaxEnt Model.
 *
 * \author vernkin
 *
 * Created on March 20, 2009, 10:46 AM
 */

#ifndef _CMAPOSTAGGER_H
#define	_CMAPOSTAGGER_H

#include "CMABasicTrainer.h"
#include "VTrie.h"
#include "strutil.h"
#include "CPPStringUtils.h"
#include "cma_ctype.h"
#include "cma_wtype.h"

#include <algorithm>
#include <math.h>
#include <vector>
#include <set>
#include <string>

using namespace maxent::me;

namespace cma{

/**
 * Get context of POS(Part of Speech) (zh/chinese)
 *
 * \param words the words vector
 * \param tags the poc tags vector
 * \param i the index in the wrods
 * \param rareWord whether this word is rareWord
 * \param context to hold the return context
 * \param ctype indicates the encoding types
 */
void get_pos_zh_scontext(vector<string>& words, vector<string>& tags, size_t i,
        bool rareWord, vector<string>& context, CMA_CType *ctype);


/**
 * Training the POS Maxent model
 *
 * \param file the source file, with formate "word1/tag1 word2/tag2 ..."
 * \param cateFile the cateFile (include the path) is the prefix of all the file
 *    that created while training
 * \param encType the encoding type, default is gb2312
 * \param extractFile if set, save the training data to the extractFile and exit
 * \param iters how many iterations are required for training[default=15]
 * \param method the method of Maximum Model Parameters Estimation [default = gis]
 * \param gaussian apply Gaussian penality when training [default=0.0]
 * \param isPOS if true, output the tag dictioanry
 */
void pos_train(const char* file, const string& cateFile,
        Knowledge::EncodeType encType = Knowledge::ENCODE_TYPE_GB2312,
        string posDelimiter = "/",
        const char* extractFile = 0, string method = "gis", size_t iters = 15,
        float gaussian = 0.0f);

/**
 * \brief to hold the objects in the N-best algorithm
 *
 */
struct POSTagUnit{
    /**
     * the current pos
     */
    string pos;

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
 * \brief Tagging the POS Information
 * Tagging the POS using the maxent model.
 */
class POSTagger{
public:
    /**
     * Construct the POSTagger with outer VTrie
     * \param model POS model name
     */
    POSTagger(const string& model, VTrie* pTrie);

    /**
     * Construct the POSTagger with inner VTrie (read from dictFile)
     * 
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
     * \param to store the result value
     */
    void tag_sentence(vector<string>& words, size_t N, size_t retSize,
            vector<pair<vector<string>, double> >& segment);

    /**
     * Only return the best result
     * \param words words vector
     * \param posRet to hold the result value
     * \param begin the begin index (include) to check
     * \param end the first index that should not check
     */
    void tag_sentence_best(vector<string>& words, vector<string>& posRet,
            int begin, int end);

    /**
     * tag the word with the best POS and return its score
     * \param words words vector
     * \param poses the previous pos vector, but the current pos won't store into
     * the poses
     * \param index the current index to check
     * \param pos to store the best pos
     * \return return the score of the best pos
     */
    double tag_word_best(vector<string>& words, vector<string>& poses, int index,
            string& pos);


    /**
     * Set the reference to the specific character encoding
     *
     * \param ctype new specific character encoding
     */
    void setCType(CMA_CType *ctype){
        ctype_ = ctype;
    }

    /**
     * Append the POS Information into Trie and POS Vector
     * \param line a line like: word1 pos1 pos2 ... posN
     * \return whether add successfully
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
            double initScore, int candidateNum, CMA_WType& wtype);

    /**
     * tag the word with the best POS (private method)
     * \param words words vector
     * \param poses the previous pos vector, but the current pos won't store into
     * the poses
     * \param index the current index to check
     * \param pos to store the best pos
     * \param wtype the identify the type of the word
     * \return return the score of the best pos
     */
    inline double tag_word_best_1(vector<string>& words, vector<string>& poses, 
            int index, string& pos, CMA_WType& wtype);

public:
    /** vector to hold the POS information */
    vector<set<string> > posVec_;

private:
    /**
     * The maxent model
     */
    MaxentModel me;
    
    /** 
     * If the trie_ passed by parameter the trie_ is not supposed destroyed by
     * the POSTagger
     */
    VTrie *trie_;

    /** Whether the trie is created by the constructor */
    bool isInnerTrie_;

    /** the encoding type */
    CMA_CType *ctype_;
};

}
#endif	/* _CMAPOSTAGGER_H */

