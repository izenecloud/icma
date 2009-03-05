/** \file crf_analyzer.h
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Mar 2, 2009
 */

#ifndef CMA_CRF_ANALYZER_H
#define CMA_CRF_ANALYZER_H

#include "analyzer.h"
#include "sentence.h"

namespace CRFPP
{
class Tagger;
} // namespace CRFPP

namespace cma
{

class CRF_Knowledge;

/**
 * CRF_Analyzer executes the Chinese morphological analysis based on conditional random field.
 */
class CRF_Analyzer : public Analyzer
{
public:
    CRF_Analyzer();

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
     * \post on successful return, the n-best results could be got by \e sentence.getListSize(), etc.
     */
    virtual int runWithSentence(Sentence& sentence);

    /**
     * Execute the morphological analysis based on a paragraph string.
     * \param inStr paragraph string
     * \return 0 for fail, otherwise a non-zero string pointer for the one-best result
     */
    virtual const char* runWithString(const char* inStr);

    /**
     * Execute the morphological analysis based on a file.
     * \param inFileName input file name
     * \param outFileName output file name
     * \return 0 for fail, 1 for success
     * \post on successful return, the one-best result is printed to file \e outFileName.
     */
    virtual int runWithStream(const char* inFileName, const char* outFileName);

private:
    /**
     * Parse the raw sentence string.
     * \param rawStr sentence string
     * \return 0 for fail, 1 for success
     */
    int parseSentence(const char* rawStr);

    /**
     * Get a candidate result of segmentation.
     * \param morphemeList the list to store the candidate result
     */
    void getSegResult(MorphemeList& morphemeList) const;

    /**
     * Print a candidate result of segmentation.
     */
    void printSegResult() const;

private:
    /** pointer to knowledge */
    CRF_Knowledge* knowledge_;

    /** tagger for segment */
    CRFPP::Tagger* segTagger_;

    /** string buffer stores result for \e runWithString */
    std::string strBuffer_;
};

} // namespace cma

#endif // CMA_CRF_ANALYZER_H
