/* 
 * 
 * \author vernkin
 */

#ifndef _CMA_ME_KNOWLEDGE_H
#define	_CMA_ME_KNOWLEDGE_H

#include "knowledge.h"
#include "CMAPOCTagger.h"
#include "CMAPOSTagger.h"

#include "VSynonym.h"

#include <set>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

namespace cma{

class CMA_ME_Knowledge : public Knowledge{

public:
    CMA_ME_Knowledge();
    virtual ~CMA_ME_Knowledge();

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
     * Load the synonym dictionary
     * \param fileName the file name
     * \return 0 for fail, 1 for success
     */
    virtual int loadSynonymDictionary(const char* fileName);

    /**
     * Get the Synonyms of the specific word
     * \param word the specific word
     * \param synomym the object to hold the synonym
     */
    virtual void getSynonyms(const string& word, VSynonym& synonym);

    /**
     * Load the stop-word dictionary file, which is in text format.
     * The words in this file are ignored in the morphological analysis result.
     * \param fileName the file name
     * \return 0 for fail, 1 for success
     */
    virtual int loadStopWordDict(const char* fileName);

    /**
     * Load the model file in binary format, which contains statistical
     * information for word segmentation.
     * \param cateName the file name, it include two files, (cateName).model and
     *      (cateName).tag. For example, with cateName cate1, "cate1.model" and
     *      "cate1.tag" should exists
     * \return 0 for fail, 1 for success
     */
    virtual int loadStatModel(const char* cateName);

    /**
     * Load the model file in binary format, which contains statistical
     * information for part-of-speech tagging.
     * \param cateName the file name, it include two files, (cateName).model and
     *      (cateName).tag. For example, with cateName cate1, "cate1.model" and
     *      "cate1.tag" should exists
     * \return 0 for fail, 1 for success
     */
    virtual int loadPOSModel(const char* cateName);

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
    SegTagger* getSegTagger() const;

    /**
     * Get the segment tagger.
     * \return pointer to tagger
     */
    POSTagger* getPOSTagger() const;

    /**
     * Whether the specific word is stop word
     */
    bool isStopWord(const string& word);

    VTrie* getTrie();

private:
    /** Input maybe a line or a word*/
    string readEncryptLine(FILE *in);

    void loadOuterDictRecord(const string& record, int counter = 1);

private:
    /** tagger for segment */
    SegTagger *segT_;

    /** tagger for POS */
    POSTagger *posT_;

    /** VSynonymContainer */
    VSynonymContainer *vsynC_;

    /** The Trie to hold system and */
    VTrie* trie_;

    /** stop words set */
    set<string> stopWords_;

};

}

#endif	/* _CMA_ME_KNOWLEDGE_H */

