/** \file CMA_ME_Knowledge.h
 * 
 * \brief The Knowledge class of the CMAC
 *
 * \author vernkin
 * \version 0.1
 * \date Feb 17, 2009
 */
#ifndef _CMA_ME_KNOWLEDGE_H
#define	_CMA_ME_KNOWLEDGE_H

#include "knowledge.h"
#include "CMAPOCTagger.h"
#include "CMAPOSTagger.h"
#include "pos_table.h"

#include "VSynonym.h"

#include <set>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

namespace cma{

/**
 * \brief Knowledge for the CMAC
 *
 */
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
     * \param synonym the object to hold the synonym
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
     * \param loadModel whether load model file, default is true
     * \return 0 for fail, 1 for success
     */
    virtual int loadStatModel(const char* cateName, bool loadModel = true );

    /**
     * Load the model file in binary format, which contains statistical
     * information for part-of-speech tagging.
     * \param cateName the file name, it include two files, (cateName).model and
     *      (cateName).tag. For example, with cateName cate1, "cate1.model" and
     *      "cate1.tag" should exists
     * \param loadModel whether load model file, default is true
     * \return 0 for fail, 1 for success
     */
    virtual int loadPOSModel(const char* cateName, bool loadModel = true );

    /**
     * Load the configuration file, which is in text format.
     * This file contains the configuration used in morphological analysis, such as part-of-speech configuration, etc.
     * \param fileName the file name
     * \return 0 for fail, 1 for success
     */
    virtual int loadConfig(const char* fileName);
    
    /**
     * Encode the system dictionary file from text to binary format.
     * \param txtFileName the text file name
     * \param binFileName the binary file name
     * \return 0 for fail, 1 for success
     */
    virtual int encodeSystemDict(const char* txtFileName, const char* binFileName);

    /**
     * Auto load POS model, Stat Model and System Dictionaries.
     * Encoding must be set here.
     * \encoding like gb18030 and utf8
     * \modelPath the directory that contains all the models
     * \param loadModel whether load model file, default is true
     * \return whether perform success
     */
    virtual int loadModel(const char* encoding, const char* modelPath,
            bool loadModel = true );

    /**
     * Whether contains POS model
     * \return true if contains POS model
     */
    virtual bool isSupportPOS() const;

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

    /**
     * Get the VTrie that Knowledge holds
     *
     * \return the VTrie that Knowledge holds
     */
    VTrie* getTrie();

    /**
     * Get POSTable
     */
    POSTable* getPOSTable() const;

    /**
	 * Get POSTable
	 */
    POSTable* getPOSTable();

    /**
     * Get System Property
     * \param key the name of that system property
     * \return if not exists, return 0.
     */
    const string* getSystemProperty( const string& key );

private:
    /** Input maybe a line or a word*/
    string readEncryptLine(FILE *in);

    /**
     * Append the POS Information into Trie and POS Vector
     * \param line a line like: word1 pos1 pos2 ... posN
     * \return whether add successfully
     */
    bool appendWordPOS(string& line);

    /**
     * Load the system dictionary file, which is in binary format.
     * \param binFileName the file name
     * \return 0 for fail, 1 for success
     */
    int loadSystemDict_(const char* binFileName);

    /**
     * Load the user dictionary file, which is in text format.
     * \param fileName the file name
     * \return 0 for fail, 1 for success
     */
    int loadUserDict_(const char* fileName);

    /**
	 * Load property config file, with format key = value
	 * \param filename the target file name
	 * \param map the return values are stored in this map
	 * \return true for success, false for failure.
	 */
	bool loadConfig0(const char *filename, map<string, string>& map, bool required = true);

private:
    /** tagger for segment */
    SegTagger *segT_;

    /** tagger for POS */
    POSTagger *posT_;

    /** VSynonymContainer */
    VSynonymContainer *vsynC_;

    /** The Trie to hold system and */
    VTrie* trie_;

    /** POS Table */
    POSTable* posTable_;

    /** stop words set */
    set<string> stopWords_;

    /** The word in blackWords_ won't be added to dictionary */
    set<string> blackWords_;

    /** The System Config (from poc.config) */
    map<string, string> sysConfig_;
};

}

#endif	/* _CMA_ME_KNOWLEDGE_H */

