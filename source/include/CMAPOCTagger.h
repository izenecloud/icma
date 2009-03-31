/* 
 * File:   CMAPOCTagger.h
 * Author: vernkin
 *
 * Created on March 24, 2009, 8:52 PM
 */

#ifndef _CMAPOCTAGGER_H
#define	_CMAPOCTAGGER_H

#include "CMABasicTrainer.h"
#include "types.h"

#include <algorithm>
#include <math.h>
#include <set>
#include <map>
using namespace maxent::me;

namespace cma{

extern map<string, uint8_t> POCs2c;

extern vector<string> POCVec;

enum CharType
{
    CHAR_TYPE_DIGIT = 1, ///< digit character
    CHAR_TYPE_PUNTUATION, ///< puntuation character
    CHAR_TYPE_LETTER, ///< letter character
    CHAR_TYPE_OTHER ///< other character
};

extern map<string, CharType> TYPE_MAP;

extern set<string> HYPHEN_SET;

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

inline bool is_matched_poc(string& preTag, string& curTag){
    if(preTag == "R" || preTag == "I")
        return curTag == "L" || curTag == "I";
        
    if(preTag == "L")
            return curTag == "R" || curTag == "M";
        
    if(preTag == "M")
            return curTag == "R" || curTag == "M";
     
    cerr<<"Invalid POC tag "<<preTag<<endl;
    exit(1);
    return false;
}

class POCTagUnit{
public:
    POCTagUnit(uint8_t pPOC, double pScore, int pIndex) : pocCode(pPOC),
            score(pScore), index(pIndex){

    }

    //TODO another copy
    bool operator < (POCTagUnit other) const{
        return score - other.score;
    }

public:
    uint8_t pocCode;
    double score;
    int index;
};


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
    void seg_sentence(string sentence, size_t N,
            vector<pair<vector<string>, double> >& segment);

    void appendWordTag(string& word, string& tag, int counter = 1);

    static void initialize();

private:

    /**
     * tag word words[i] under given tag history hist
     *
     * \return a list of (tag, score) pair sorted
     */
    void tag_word(vector<string>& words, int i, size_t N, vector<string>& hist,
        vector<POCTagUnit>& ret, double initScore);

private:
    MaxentModel me;
    /** tag dictionary */
    map<string, map<string, int> > tagDict_;
    context_t get_context;
};

}
#endif	/* _CMAPOCTAGGER_H */

