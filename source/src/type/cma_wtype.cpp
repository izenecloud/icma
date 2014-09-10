/** \file cma_wtype.cpp
 * \brief The class the identify the type of the word
 * \author Jun Jiang
 * \version 0.1
 * \date Mar 23, 2009
 */

#include "icma/type/cma_wtype.h"
#include "icma/type/cma_ctype.h"

#include <cassert>

using namespace std;

//#define ENABLE_WT_ASSERT

namespace cma
{
CMA_WType::WordType CTYPE_2_WTYPE[ CHAR_TYPE_NUM ] =
{
    CMA_WType::WORD_TYPE_OTHER, // CHAR_TYPE_INIT
    CMA_WType::WORD_TYPE_OTHER, // CHAR_TYPE_OTHER
    CMA_WType::WORD_TYPE_NUMBER,// CHAR_TYPE_DIGIT
    CMA_WType::WORD_TYPE_NUMBER,// CHAR_TYPE_CHARDIGIT
    CMA_WType::WORD_TYPE_PUNC,  // CHAR_TYPE_PUNC
    CMA_WType::WORD_TYPE_PUNC,  // CHAR_TYPE_SPACE
    CMA_WType::WORD_TYPE_DATE,  // CHAR_TYPE_DATE
    CMA_WType::WORD_TYPE_LETTER // CHAR_TYPE_LETTER
};


CMA_WType::CMA_WType(const CMA_CType* ctype)
    : ctype_(ctype), tokenizer_(ctype)
{
}


CMA_WType::WordType CMA_WType::getWordType(const char* word)
{
    assert(word);

    CharType preType = CHAR_TYPE_INIT;
    CharType curType = CHAR_TYPE_INIT;

    tokenizer_.assign(word);
    const char* nextP = tokenizer_.next();
    char curChar[5];
    while(nextP)
    {
        strcpy(curChar, nextP);
        nextP = tokenizer_.next();
        curType = ctype_->getCharType(curChar, preType, nextP);

        if(preType == curType)
            continue;

        switch(curType)
        {
        case CHAR_TYPE_OTHER:
            return WORD_TYPE_OTHER;
        case CHAR_TYPE_PUNC:
            if(preType == CHAR_TYPE_INIT && !nextP)
                return WORD_TYPE_PUNC;
            return WORD_TYPE_OTHER;
        default:
            break;
        }

        switch(preType)
        {
        case CHAR_TYPE_INIT:
            preType = curType;
            break;

        case CHAR_TYPE_DIGIT:
        case CHAR_TYPE_CHARDIGIT:
            switch(curType)
            {
            case CHAR_TYPE_LETTER:
                preType = CHAR_TYPE_LETTER;
                break;
            case CHAR_TYPE_DATE:
                return (nextP) ? WORD_TYPE_OTHER : WORD_TYPE_DATE;
            default:
            {
#ifdef ENABLE_WT_ASSERT
                string errorMsg("unexpected arrive here (pre is number, cur is not-letter or non-date). Input: ");
                errorMsg += word;
                assert(false && errorMsg.c_str());
#endif
                break;
            }
            }
            break;

        case CHAR_TYPE_LETTER:
            if(curType == CHAR_TYPE_DIGIT || curType == CHAR_TYPE_CHARDIGIT)
                continue;
#ifdef ENABLE_WT_ASSERT
            assert(false && "unexpected arrive here (pre is Letter and cur is non-digit)");
#endif
            break;

        default:
#ifdef ENABLE_WT_ASSERT
            assert(false && "unexpected arrive here (preType)");
#endif
            break;
        }
    }

    switch(preType)
    {
    case CHAR_TYPE_LETTER:
        return WORD_TYPE_LETTER;
    case CHAR_TYPE_DIGIT:
    case CHAR_TYPE_CHARDIGIT:
        return WORD_TYPE_NUMBER;
    default:
#ifdef ENABLE_WT_ASSERT
        assert(false && "unexpected arrive here");
#endif
        return WORD_TYPE_OTHER;
    }
    return WORD_TYPE_OTHER;
}


CMA_WType::WordType CMA_WType::getWordType(CharType* types, size_t beginIdx, size_t endIdx)
{
    size_t maxIdx = endIdx - 1;
    CharType type = types[ maxIdx ];
    if( type == CHAR_TYPE_DIGIT && maxIdx > beginIdx )
    {
        for( size_t i = beginIdx; i < maxIdx; ++i )
        {
            if( types[ i ] != CHAR_TYPE_DIGIT )
            {
                type = types[ i ];
                break;
            }
        }
    }

    return CTYPE_2_WTYPE[ type ];
}

bool CMA_WType::isLetterMixNumber(CharType* types, size_t beginIdx, size_t endIdx)
{
    for(size_t i = beginIdx; i < endIdx; ++i)
    {
        if (types[ i ] == CHAR_TYPE_DIGIT)
        {
            return true;
        }
    }

    return false;
}

} // namespace cma
