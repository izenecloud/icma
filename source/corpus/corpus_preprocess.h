/** \file corpus_preprocess.h
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Feb 23, 2009
 */

#ifndef CMA_CORPUS_PREPROCESS_H
#define CMA_CORPUS_PREPROCESS_H

#include <string>

namespace cma
{

/**
 * CorpusPreprocess preprocsses the corpus for training statistical model.
 */
class CorpusPreprocess
{
public:
    virtual ~CorpusPreprocess();

    /**
     * Read from \e rawFile, process it, and write into \e resultFile in a specific format.
     * \param rawFile the file to be processed
     * \param resultFile the output file
     */
    virtual void format(const char* rawFile, const char* resultFile) = 0;


    /** Output format of dictionary */
    enum Dict_Format
    {
        DICT_FORMAT_WORDPOS, ///< "word pos1 pos2 ..."
        DICT_FORMAT_POSWORD, ///< "pos word1 word2 ..."
        DICT_FORMAT_NUM ///< total number of format
    };

    /**
     * Read from \e corpusFile, extract pairs of word and part-of-speech, and write the dictionary into \e dictFile.
     * \param corpusFile the corpus file, which format is assumed as "word/pos" delimited by spaces
     * \param dictFile the dictionary file for output
     * \param format the format of dictionary file
     */
    static void extractDictionary(const char* corpusFile, const char* dictFile, Dict_Format format);

protected:
    static const std::string FILE_OPEN_ERROR; ///< error message when file open failed
};

} // namespace cma

#endif // CMA_CORPUS_PREPROCESS_H
