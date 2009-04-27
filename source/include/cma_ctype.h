/** \file cma_ctype.h
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Mar 10, 2009
 */

#ifndef CMA_CTYPE_H
#define CMA_CTYPE_H

#include <string>

using std::string;

#include "knowledge.h" // Knowledge::EncodeType
#include "cmacconfig.h"

namespace cma
{

/**
 * Character type.
 */
enum CharType
{
    CHAR_TYPE_INIT, ///< initial type
    CHAR_TYPE_NUMBER, ///< digit character
    CHAR_TYPE_PUNC, ///< puntuation character
    CHAR_TYPE_SPACE, ///< space character, like ' '
    CHAR_TYPE_DATE, ///< date character
    CHAR_TYPE_LETTER, ///< letter character
    CHAR_TYPE_OTHER, ///< other character
    CHAR_TYPE_NUM
};

/**
 * CMA_CType gives the character type information.
 */
class CMA_CType
{
public:
    /**
     * Create an instance of \e CMA_CType based on the character encode type.
     * \param type the character encode type
     * \return the pointer to instance
     */
    static CMA_CType* instance(Knowledge::EncodeType type);

    static Knowledge::EncodeType getEncType(string encType);


    virtual ~CMA_CType();

    /**
     * Get the byte count of the first character pointed by \e p, which
     * character is in a specific encoding.
     * \param p pointer to the character string
     * \return true for punctuation, false for non punctuation.
     */
    virtual unsigned int getByteCount(const char* p) const = 0;

    /**
     * Get the character type.
     * \param p pointer to the string to be checked
     * \param preType the chartype of the previous character
     * \param nextP the pointer of the next character, it can be 0
     * \return the character type.
     */
    virtual CharType getCharType(const char* p, CharType preType,
            const char* nextP) const = 0;

    bool isPunct(const char* p) const;
};

} // namespace cma

#endif // CMA_CTYPE_H
