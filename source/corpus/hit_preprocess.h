/** \file hit_preprocess.h
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Feb 24, 2009
 */

#ifndef CMA_HIT_PREPROCESS_H
#define CMA_HIT_PREPROCESS_H

#include "corpus_preprocess.h"

namespace cma
{

/**
 * HIT_Preprocess preprocsses the corpus "HIT_哈工大".
 */
class HIT_Preprocess : public CorpusPreprocess
{
public:
    /**
     * Read from \e rawFile, extract segmented sentences, and write into \e resultFile with specific format.
     * An example of input and output is like below:
     * Input from raw file:
     *	    我们即将动身。
     *	    [1]我们/r [2]即将/d [3]动身/v [4]。/wp [5]<EOS>/<EOS>
     *	    [3]动身_[2]即将  [3]动身_[1]我们  [5]<EOS>_[3]动身
     * Output to result file:
     *	    我们/r 即将/d 动身/v 。/wp 
     * \param rawFile the file to be processed
     * \param resultFile the output file
     */
    virtual void format(const char* rawFile, const char* resultFile);
};

} // namespace cma

#endif // CMA_HIT_PREPROCESS_H
