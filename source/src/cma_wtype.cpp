/** \file cma_wtype.cpp
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Mar 23, 2009
 */

#include "cma_wtype.h"
#include "cma_ctype.h"

#include <cassert>

using namespace std;

namespace cma
{

CMA_WType::CMA_WType(const CMA_CType* ctype)
    : ctype_(ctype), tokenizer_(ctype)
{
}


CMA_WType::WordType CMA_WType::getWordType(const char* word)
{
    assert(word);

    CharType preType = CHAR_TYPE_INIT;
    CharType curType;

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

        switch(curType){
            case CHAR_TYPE_OTHER:
                return WORD_TYPE_OTHER;
            case CHAR_TYPE_PUNC:
                if(preType == CHAR_TYPE_INIT && !nextP)
                    return WORD_TYPE_PUNC;
                return WORD_TYPE_OTHER;
            default:
                break;
        }

        switch(preType){
            case CHAR_TYPE_INIT:
                preType = curType;
                break;

            case CHAR_TYPE_NUMBER:
                switch(curType){
                    case CHAR_TYPE_LETTER:
                        preType = CHAR_TYPE_LETTER;
                        break;
                    case CHAR_TYPE_DATE:
                        return (nextP) ? WORD_TYPE_OTHER : WORD_TYPE_DATE;
                    default:
                        assert(false && "unexpected arrive here (pre is number, cur is not-letter or non-date)");
                        break;
                }
                break;

            case CHAR_TYPE_LETTER:
                if(curType == CHAR_TYPE_NUMBER)
                    continue;              
                assert(false && "unexpected arrive here (pre is Letter and cur is non-digit)");
                break;
                
            default:
                assert(false && "unexpected arrive here (preType)");
                break;
        }
    }

    switch(curType){
        case CHAR_TYPE_LETTER:
            preType = CHAR_TYPE_LETTER;
            break;
        case CHAR_TYPE_NUMBER:
            return WORD_TYPE_NUMBER;
        default:
            assert(false && "unexpected arrive here");
            return WORD_TYPE_OTHER;
    }
    return WORD_TYPE_OTHER;
}

} // namespace cma
