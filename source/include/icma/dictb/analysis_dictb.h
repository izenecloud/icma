/*
 * \file analysis_dictb.h
 * \brief the header file for analysis_dictb approach
 * \date Jun 8, 2010
 * \author Vernkin Chen
 */

#ifndef ANALYSIS_DICTB_H_
#define ANALYSIS_DICTB_H_

#include <vector>
#include <string>

#include "icma/type/cma_ctype.h"
#include "icma/type/cma_wtype.h"
#include "VTrie.h"

namespace cma
{
namespace dictb
{

class DictBString
{


public:
    size_t beginIdx_; ///< include
    size_t endIdx_;   ///< exclude
    std::vector< std::string >* pData_; ///< The Character Vector
    cma::CMA_WType::WordType fragType_;

};

void parseDictBString(
        vector< DictBString >& out,
        vector<string>& words,
        CharType* types,
        VTrie* trie
        );

}

}

#endif /* ANALYSIS_DICTB_H_ */
