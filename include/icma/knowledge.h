/** \file knowledge.h
 * \brief Knowledge manages the linguistic information for Chinese morphological analysis.
 * \author Jun Jiang
 * \version 0.1
 * \date Feb 17, 2009
 */

#ifndef CMA_KNOWLEDGE_H
#define CMA_KNOWLEDGE_H

#include <vector>
#include <string>

namespace cma
{

/**
 * \brief Knowledge manages the linguistic information for Chinese morphological analysis.
 *
 */
class Knowledge
{
public:
    Knowledge();
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
     * Load the model file in binary format, which contains statistical information for word segmentation.
     * \param binFileName the file name
     * \param loadModel whether load model file, default is true
     * \return 0 for fail, 1 for success
     */
    virtual int loadStatModel(const char* binFileName, bool loadModel = true) = 0;

    /**
     * Load the model file in binary format, which contains statistical information for part-of-speech tagging.
     * \param binFileName the file name
     * \param loadModel whether load model file, default is true
     * \return 0 for fail, 1 for success
     */
    virtual int loadPOSModel(const char* binFileName, bool loadModel = true) = 0;

    /**
     * Load the configuration file, which is in text format.
     * This file contains the configuration used in morphological analysis, such as part-of-speech configuration, etc.
     * \param fileName the file name
     * \return 0 for fail, 1 for success
     */
    virtual int loadConfig(const char* fileName) = 0;

    /**
     * Encode the system dictionary file from text to binary format.
     * \param txtFileName the text file name
     * \param binFileName the binary file name
     * \return 0 for fail, 1 for success
     */
    virtual int encodeSystemDict(const char* txtFileName, const char* binFileName) = 0;

    /**
     * Encode type of characters.
     */
    enum EncodeType
    {
	ENCODE_TYPE_GB2312, ///< GB 2312 character type
	ENCODE_TYPE_BIG5, ///< Big 5 character type
    ENCODE_TYPE_GB18030, ///< GB18030 character type
    ENCODE_TYPE_UTF8, ///< UTF8 character type
#ifdef USE_UTF_16
    ENCODE_TYPE_UTF16, ///< UTF16 character type
#endif
	ENCODE_TYPE_NUM ///< the count of character types
    };

    /**
     * Set the character encode type.
     * If this function is not called, the default value returned by \e getEncodeType() is \e ENCODE_TYPE_GB2312.
     * \param type the encode type
     */
    void setEncodeType(EncodeType type);

    /**
     * Get the character encode type.
     * \return the encode type
     */
    EncodeType getEncodeType() const;

    /**
     * Get the encoding type by the encode type string
     *
     * \param encType encode type string
     * \return assicated Knowledge::EncodeType in the class Knowledge
     */
    static Knowledge::EncodeType decodeEncodeType(const char* encType);

    /**
     * Auto load POS model, Stat Model and System Dictionaries.
     * Encoding must be set here.
     * \modelPath the directory that contains all the models, the last
     * directory of the modelPath must be encdoing name
     * \param toLoadModel whether load model file, default is true
     * \return whether perform success
     */
    int loadModel( const char* modelPath, bool toLoadModel = true );

    /**
     * Auto load POS model, Stat Model and System Dictionaries.
     * Encoding must be set here.
     * \encoding like gb18030 and utf8
     * \modelPath the directory that contains all the models
     * \param toLoadModel whether load model file, default is true
     * \return whether perform success
     */
    virtual int loadModel(const char* encoding, const char* modelPath,
            bool toLoadModel = true) = 0;

    /**
     * Whether contains POS model
     * \return true if contains POS model
     */
    virtual bool isSupportPOS() const = 0;

    /*
     * Check whether the parameter is an exist word in the dictionary or not
     * \param word the word to be checked
     * \return whether the parameter is an exist word in the dictionary or not
     */
    virtual bool isExistWord( const char* word ) = 0;

    /**
     * Disable the specific words in the dictionary, those
     * OOV words would be ignored
     * \param words the target words
     */
    virtual void disableWords( const std::vector< std::string >& words ) = 0;

    /**
     * Enable the specific words in the dictionary, those
     * OOV words would be ignored
     * \param words the target words
     */
    virtual void enableWords( const std::vector< std::string >& words ) = 0;

private:
    /** character encode type */
    EncodeType encodeType_;
};

}

#endif // CMA_KNOWLEDGE_H
