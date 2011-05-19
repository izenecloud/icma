/** \file CMA_ME_Analyzer.h
 * \brief Analyzer for the CMAC
 * 
 * \author vernkin
 *
 */

#ifndef _CMA_ME_ANALYZER_H
#define	_CMA_ME_ANALYZER_H

#include "icma/analyzer.h"
#include "icma/sentence.h"

#include "icma/cmacconfig.h"
#include "icma/me/CMA_ME_Knowledge.h"
#include "icma/type/cma_ctype.h"

#include <string>

using namespace std;

namespace cma
{

/**
 * Analyzer Option
 */
typedef struct Option {
    bool isMaxMatch;
	bool doUnigram;  // whether integrate segmentation result with unigram
	bool useMaxOffset; // *reserved, whether set the segment's offset as the maximal offset of the unigrams it contained
	bool noOverlap; // without any overlap, generate n-best.
} AnalOption;

/**
 * \brief Analyzer for the CMAC
 *
 */
class CMA_ME_Analyzer : public Analyzer
{
public:

    CMA_ME_Analyzer();

    /**
     * The Desconstrutor won't delete knowledge pointer
     */
    ~CMA_ME_Analyzer();

    /**
     * Set the option value for analysis.
     * \param nOption the option type
     * \param nValue the option value
     * \attention when \e nOption is \e OPTION_TYPE_NBEST, the invalid \e nValue less than 1 will take no effect.
     */
    virtual void setOption(OptionType nOption, double nValue);

    /**
     * Set the \e Knowledge for analysis.
     * \param pKnowledge the pointer of \e Knowledge
     */
    virtual void setKnowledge(Knowledge* pKnowledge);

    /**
     * Execute the morphological analysis based on a sentence.
     * \param sentence an instance of \e Sentence
     * \return 0 for fail, 1 for success
     * \pre the raw sentence string could be got by \e sentence.getString().
     * \post on successful return, the n-best results could be got by \e
     *      sentence.getListSize(), etc.
     * \attention white-space characters are ignored in the analysis, which
     *      include " \t\n\v\f\r", and also space character in specific encoding.
     */
    virtual int runWithSentence(Sentence& sentence);

    /**
     * Execute the morphological analysis based on a paragraph string.
     * \param inStr paragraph string
     * \return 0 for fail, otherwise a non-zero string pointer for the one-best
     *      result
     */
    virtual const char* runWithString(const char* inStr);

    /**
     * Execute the morphological analysis based on a file.
     * \param inFileName input file name
     * \param outFileName output file name
     * \return 0 for fail, 1 for success
     * \post on successful return, the one-best result is printed to file
     *      \e outFileName.
     */
    virtual int runWithStream(const char* inFileName, const char* outFileName);

    /**
	 * Get the N-Gram result in the inStr
	 * \param inStr paragraph string
	 * \param n the specific n in the N-Gram
	 * \param output to keep the output value
	 */
	virtual void getNGramResult( const char* inStr, int n, vector<string>& output );

	/**
	 * Get all the specific N-Gram results (see parameter nArray) in the inStr
	 * \param inStr paragraph string
	 * \param nArray the collection of the specific n in the N-Gram
	 * \param output to keep the output value
	 */
	virtual void getNGramArrayResult( const char* inStr, vector<int> nArray, vector<string>& output );

    /**
     * Split a paragraph string into sentences.
     * \param paragraph paragraph string
     * \param sentences sentence vector
     * \attention the original elements in \e sentences would not be removed, and the splited sentences are appended into \e sentences.
     */
    virtual void splitSentence(const char* paragraph, std::vector<Sentence>& sentences);

    /**
     * Reset all the POS index Information as defVal
     * \return defVal default value to reset (default is true)
     */
    virtual void resetIndexPOSList( bool defVal = true );

    /**
     * Set the POS index as true in the posList
     * \param posList the list that allowed in the posList
     * \return how many POS are set successfully (mainly because not exists).
     */
    virtual int setIndexPOSList( std::vector<std::string>& posList );

    /**
     * Get the POS index code from the POS string in the global part-of-speech table.
     * \param pos the POS string
     * \return POS index code, -1 for non POS available
     */
    virtual int getCodeFromStr(const std::string& pos) const;

    /**
     * Get the POS string from the POS index code in the global part-of-speech table.
     * \param index the POS index code
     * \return POS string, null pointer for non POS available
     */
    virtual const char* getStrFromCode(int index) const;

    /**
	 * Get the number of POS tags in the global part-of-speech table.
	 * \return the number of POS tags
	 */
	virtual int getPOSTagSetSize() const;

    /**
     * Whether the specific word is stop word
     */
    virtual bool isStopWord(const string& word)
    {
        if (knowledge_)
            return knowledge_->isStopWord(word);

        return false;
    }

    typedef string OneGramType;

    typedef void(CMA_ME_Analyzer::*analysis_t)(
    		AnalOption& analOption,
            const char*,
            int,
            Sentence&,
            bool
            );

private:
    /**
     * Each segment only map to one pos set, with maximum model approach
     */
    void analysis_mmmodel(
    		AnalOption& analOption,
            const char* sentence,
            int N,
            Sentence& ret,
            bool tagPOS = true
            );

    /**
     * Each segment with pure Maximum Model Approach
     */
    void analysis_pure_mmmodel(
    	AnalOption& analOption,
        const char* sentence,
        int N,
        Sentence& ret,
        bool tagPOS = true
        );

    /**
     * Forwards maximum matching approach
     */
    void analysis_fmm(
    		AnalOption& analOption,
            const char* sentence,
            int N,
            Sentence& ret,
            bool tagPOS = true
            );

    /**
     * Dictionary based nbest approach
     */
    void analysis_dictb(
    		AnalOption& analOption,
            const char* sentence,
            int N,
            Sentence& ret,
            bool tagPOS = true
            );

    /**
     * Dictionary based Forwards minimum conver segmentation
     */
    void analysis_fmincover(
    		AnalOption& analOption,
            const char* sentence,
            int N,
            Sentence& ret,
            bool tagPOS = true
            );

    /**
     * Simply combine sequential letters, digits and letters+digits together
     * \param sentence the input string
     * \param output the string is the input value and bool represents whether
     */
    void splitToOneGram( const char* sentence, vector<vector<OneGramType> >& output );

    /**
     * Implementation of getting N-Gram result
     */
    void getNGramResultImpl( const vector<vector<OneGramType> >& oneGram, const int n, vector<string>& output );

    void extractCharacter( const char* sentence, StringVectorType& charOut );

    /**
     * \param types should be allocated enough memory before invoking
     */
    void setCharType( StringVectorType& charIn, CharType* types );

    void createStringLexicon(
            StringVectorType& words,
            PGenericArray<size_t>& segSeq,
            StringVectorType& out,
            size_t beginIdx,
            size_t endIdx
            );

    void createWordOffset(
            PGenericArray<size_t>& segSeq,
            size_t beginIdx,
            size_t endIdx,
            PGenericArray<size_t>& out
            );

private:
    CMA_ME_Knowledge *knowledge_;

    /** string buffer stores result for \e runWithString */
    string strBuf_;

    /**
     * The CMA_CType object to keep the encoding
     */
    CMA_CType* ctype_;

    /**
     * The Encode Type of Analyzer
     */
    Knowledge::EncodeType encodeType_;

    POSTable* posTable_;

    /**
     * Analysis Type
     */
    CMA_ME_Analyzer::analysis_t analysis;

    /**
     * options for Analyzer
     */
    AnalOption analOption_;
};


}


#endif	/* _CMA_ME_ANALYZER_H */

