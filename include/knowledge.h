/** \file knowledge.h
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Feb 17, 2009
 */

#ifndef CMA_KNOWLEDGE_H
#define CMA_KNOWLEDGE_H

namespace cma
{

/**
 * Knowledge manages the linguistic information for Chinese morphological analysis.
 */
class Knowledge
{
public:
    virtual ~Knowledge();

    /**
     * Load the system dictionary file, which is in binary format.
     * \param binFileName the file name
     * \return 0 for fail, 1 for success
     */
    virtual int loadSystemDict(const char* binFileName) = 0;

    /**
     * Load the user dictionary file, which is in text format.
     * \param fileName the file name
     * \return 0 for fail, 1 for success
     */
    virtual int loadUserDict(const char* fileName) = 0;

    /**
     * Load the stop-word dictionary file, which is in text format.
     * The words in this file are ignored in the morphological analysis result.
     * \param fileName the file name
     * \return 0 for fail, 1 for success
     */
    virtual int loadStopWordDict(const char* fileName) = 0;

    /**
     * Load the statistical model file, which is in binary format.
     * \param binFileName the file name
     * \return 0 for fail, 1 for success
     */
    virtual int loadStatModel(const char* binFileName) = 0;

    /**
     * Encode the system dictionary file from text to binary format.
     * \param txtFileName the text file name
     * \param binFileName the binary file name
     * \return 0 for fail, 1 for success
     */
    virtual int encodeSystemDict(const char* txtFileName, const char* binFileName) = 0;
};

}

#endif // CMA_KNOWLEDGE_H
