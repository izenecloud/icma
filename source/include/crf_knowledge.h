/** \file crf_knowledge.h
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Mar 2, 2009
 */

#ifndef CMA_CRF_KNOWLEDGE_H
#define CMA_CRF_KNOWLEDGE_H

#include "knowledge.h"

namespace CRFPP
{
class Tagger;
} // namespace CRFPP

namespace cma
{

/**
 * CRF_Knowledge manages the linguistic information for Chinese morphological analysis.
 */
class CRF_Knowledge : public Knowledge
{
public:
    CRF_Knowledge();
    virtual ~CRF_Knowledge();

    /**
     * Load the system dictionary file, which is in binary format.
     * \param binFileName the file name
     * \return 0 for fail, 1 for success
     */
    virtual int loadSystemDict(const char* binFileName);

    /**
     * Load the user dictionary file, which is in text format.
     * \param fileName the file name
     * \return 0 for fail, 1 for success
     */
    virtual int loadUserDict(const char* fileName);

    /**
     * Load the stop-word dictionary file, which is in text format.
     * The words in this file are ignored in the morphological analysis result.
     * \param fileName the file name
     * \return 0 for fail, 1 for success
     */
    virtual int loadStopWordDict(const char* fileName);

    /**
     * Load the statistical model file, which is in binary format.
     * \param binFileName the file name
     * \return 0 for fail, 1 for success
     */
    virtual int loadStatModel(const char* binFileName);

    /**
     * Encode the system dictionary file from text to binary format.
     * \param txtFileName the text file name
     * \param binFileName the binary file name
     * \return 0 for fail, 1 for success
     */
    virtual int encodeSystemDict(const char* txtFileName, const char* binFileName);

    /**
     * Get the segment tagger.
     * \return pointer to tagger
     */
    CRFPP::Tagger* getSegTagger() const;
private:
    /** tagger for segment */
    CRFPP::Tagger* segTagger_;
};

}

#endif // CMA_CRF_KNOWLEDGE_H
