/** \file cma_wtype.cpp
 * \brief The class the identify the type of the word
 * \author Jun Jiang
 * \version 0.1
 * \date Mar 23, 2009
 */

#include "cma_wtype.h"
#include "cma_ctype.h"

#include <cassert>

using namespace std;

//#define ENABLE_WT_ASSERT

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
            case CHAR_TYPE_CHARDIGIT:
                switch(curType){
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
                if(curType == CHAR_TYPE_NUMBER || curType == CHAR_TYPE_CHARDIGIT)
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

    switch(preType){
        case CHAR_TYPE_LETTER:
            return WORD_TYPE_LETTER;
        case CHAR_TYPE_NUMBER:
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


CMA_WType::WordType CMA_WType::getWordType(CharType* types, size_t begin, size_t size)
{
	CharType preType = begin > 0 ? types[begin-1] : CHAR_TYPE_INIT;
	CharType curType = CHAR_TYPE_INIT;
	for(; begin < size; ++begin)
    {
        curType = types[begin];

        if(preType == curType)
            continue;

        switch(curType){
            case CHAR_TYPE_OTHER:
                return WORD_TYPE_OTHER;
            case CHAR_TYPE_PUNC:
                if(preType == CHAR_TYPE_INIT && begin>= size)
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
            case CHAR_TYPE_CHARDIGIT:
                switch(curType){
                    case CHAR_TYPE_LETTER:
                        preType = CHAR_TYPE_LETTER;
                        break;
                    case CHAR_TYPE_DATE:
                        return (begin>= size) ? WORD_TYPE_OTHER : WORD_TYPE_DATE;
                    default:
#ifdef ENABLE_WT_ASSERT
                        assert(false && "unexpected arrive here (pre is number, cur is not-letter or non-date)");
#endif
                        break;
                }
                break;

            case CHAR_TYPE_LETTER:
                if(curType == CHAR_TYPE_NUMBER || curType == CHAR_TYPE_CHARDIGIT)
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

    switch(preType){
        case CHAR_TYPE_LETTER:
            return WORD_TYPE_LETTER;
        case CHAR_TYPE_NUMBER:
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

} // namespace cma
