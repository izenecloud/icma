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
#include <iostream>

#include "icma/type/cma_ctype.h"
#include "VTrie.h"

namespace cma
{
namespace dictb
{

class DictBString
{
public:
    enum DBSType
    {
        NORMAL,
        SINGLE, ///< only have one-best (exclude one-character word)
        DIGIT,
        CHARDIGIT,
        LETTER,
        DATE,
        PUNCTUATION,
        COMPOSITE ///< begin with letter, digit
    };

    void print( vector<string>* words,  ostream& stream );

public:
    size_t beginIdx_; ///< include
    size_t endIdx_;   ///< exclude
    DBSType fragType_;

};

void parseDictBString(
        vector< DictBString >& out,
        vector<string>& words,
        CharType* types,
        VTrie* trie,
        size_t beginIdx,
        size_t endIdx
        );

}

}

#endif /* ANALYSIS_DICTB_H_ */
