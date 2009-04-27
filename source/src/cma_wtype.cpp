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

bool CMA_WType::isPunct(const char* word)
{
    assert(word);
    
    if(*word == 0)
        return false;

    tokenizer_.assign(word);
    for(const char* p=tokenizer_.next(); p; p=tokenizer_.next())
    {
        if(! ctype_->isPunct(p))
            return false;
    }

    return true;
}

CMA_WType::WordType CMA_WType::getWordType(const char* word)
{
    assert(word);

    bool start = true;
    CharType preType = CHAR_TYPE_INIT;
    assert(false && "the pretype is not correctly");
    CharType charType;
    WordType wordType = WORD_TYPE_OTHER;

    tokenizer_.assign(word);
    for(const char* p=tokenizer_.next(); p; p=tokenizer_.next())
    {
        charType = ctype_->getCharType(p, preType, 0);
        if(start)
        {
            switch(charType)
            {
                case CHAR_TYPE_NUMBER:
                    wordType = WORD_TYPE_NUMBER;
                    break;

                case CHAR_TYPE_DATE:
                    wordType = WORD_TYPE_OTHER;
                    break;

                case CHAR_TYPE_LETTER:
                    wordType = WORD_TYPE_LETTER;
                    break;

                case CHAR_TYPE_OTHER:
                    wordType = WORD_TYPE_OTHER;
                    break;

                default:
                    assert(false && "unkown character type in getting word type");
                    break;
            }

            start = false;
        }
        else
        {
            switch(wordType)
            {
                case WORD_TYPE_NUMBER:
                    switch(charType)
                    {
                        case CHAR_TYPE_NUMBER:
                            wordType = CMA_WType::WORD_TYPE_NUMBER;
                            break;

                        case CHAR_TYPE_DATE:
                            wordType = WORD_TYPE_DATE;
                            break;

                        case CHAR_TYPE_LETTER:
                            wordType = WORD_TYPE_LETTER;
                            break;

                        case CHAR_TYPE_OTHER:
                            wordType = WORD_TYPE_OTHER;
                            break;

                        default:
                            assert(false && "unkown character type in getting word type");
                            break;
                    }
                    break;

                case WORD_TYPE_DATE:
                    // date word should not be followed by any other character
                    wordType = WORD_TYPE_OTHER;
                    break;

                case WORD_TYPE_LETTER:
                    // letter word includes numbers, letters and punctuations.
                    if(charType != CHAR_TYPE_NUMBER
                            && charType != CHAR_TYPE_LETTER
                            && !ctype_->isPunct(p))
                    {
                            wordType = WORD_TYPE_OTHER;
                    }
                    break;

                case WORD_TYPE_OTHER:
                default:
                    assert(false && "unexpected word type in gettting word type");
                    break;
            }
        }

        if(wordType == WORD_TYPE_OTHER)
            break;
    }

    return wordType;
}

} // namespace cma
