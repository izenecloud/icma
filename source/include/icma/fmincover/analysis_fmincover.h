/**
 * \file analysis_fmincover.h
 * \brief 
 * \date Jul 7, 2010
 * \author Vernkin Chen
 */

#ifndef ANALYSIS_FMINCOVER_H_
#define ANALYSIS_FMINCOVER_H_

#include <vector>
#include <string>
#include <iostream>

#include "icma/type/cma_ctype.h"
#include "icma/cmacconfig.h"
#include "VTrie.h"


namespace cma
{
namespace fmincover
{

void parseFMinCoverString(
        StringVectorType& out,
        StringVectorType& words,
        CharType* types,
        VTrie* trie,
        size_t beginIdx,
        size_t endIdx
        );

}
}

#endif /* ANALYSIS_FMINCOVER_H_ */
