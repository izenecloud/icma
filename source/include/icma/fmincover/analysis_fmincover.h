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

#include "icma/me/CMA_ME_Analyzer.h"
#include "icma/type/cma_ctype.h"
#include "icma/cmacconfig.h"
#include "VTrie.h"


namespace cma
{
namespace fmincover
{

typedef PGenericArray<size_t> FMinCOutType;

void parseFMinCoverString(
        FMinCOutType& out,
        StringVectorType& words,
        CharType* types,
        VTrie* trie,
        size_t beginIdx,
        size_t endIdx,
        AnalOption& analOption
        );

}
}

#endif /* ANALYSIS_FMINCOVER_H_ */
