/** \file CMA_ME_Analyzer.h
 * \brief Analyzer for the CMAC
 * 
 * \author vernkin
 *
 */

#ifndef _CMA_ME_ANALYZER_H
#define	_CMA_ME_ANALYZER_H

#include "cmacconfig.h"
#include "analyzer.h"
#include "CMA_ME_Knowledge.h"
#include "sentence.h"
#include "cma_ctype.h"

#include <string>

using namespace std;

namespace cma
{

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
     * Split a paragraph string into sentences.
     * \param paragraph paragraph string
     * \param sentences sentence vector
     * \attention the original elements in \e sentences would not be removed, and the splited sentences are appended into \e sentences.
     */
    virtual void splitSentence(const char* paragraph, std::vector<Sentence>& sentences);

private:
    /**
     * Each segment only map to one pos set
     */
    void analysis(const char* sentence, int N, vector<vector<string> >& pos,
            vector<pair<vector<string>, double> >& segment, bool tagPOS = true);

private:
    CMA_ME_Knowledge *knowledge_;

    /** string buffer stores result for \e runWithString */
    string strBuf_;

    /**
     * The CMA_CType object to keep the encoding
     */
    CMA_CType* ctype_;
};


}


#endif	/* _CMA_ME_ANALYZER_H */

