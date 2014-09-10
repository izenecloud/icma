/** \file pfr_preprocess.h
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Feb 24, 2009
 */

#ifndef CMA_PFR_PREPROCESS_H
#define CMA_PFR_PREPROCESS_H

#include "corpus_preprocess.h"

namespace cma
{

/**
 * PFR_Preprocess preprocsses the corpus "PFR_199801_人民日报标注语料".
 */
class PFR_Preprocess : public CorpusPreprocess
{
public:
    /**
     * Read from \e rawFile, remove the phrase tags, and write into \e resultFile.
     * An example of input and output is like below:
     * Input from raw file:
     *	    [中央/n 人民/n 广播/vn 电台/n]nt 
     * Output to result file:
     *	    中央/n  人民/n  广播/vn  电台/n
     * \param rawFile the file to be processed
     * \param resultFile the output file
     */
    virtual void format(const char* rawFile, const char* resultFile);
};

} // namespace cma

#endif // CMA_PFR_PREPROCESS_H
