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
    CHAR_TYPE_HYPHEN, ///< hythen character, like '-'
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
     * Check whether is punctuation.
     * \param p pointer to the character string
     * \return true for punctuation, false for non punctuation.
     */
    virtual bool isPunct(const char* p) const = 0;


    /**
     * Get the character type.
     * \param p pointer to the string to be checked
     * \param preType the chartype of the previous character
     * \return the character type.
     */
    virtual CharType getCharType(const char* p, CharType preType) const = 0;

    /**
     * Check whether is white-space character.
     * White-space characters are " \t\n\v\f\r", and also space character in specific encoding.
     * \param p pointer to the character string
     * \return true for white-space character, false for non white-space character.
     */
    virtual bool isSpace(const char* p) const = 0;

    /**
     * Check whether is a seperator of sentence.
     * \param p pointer to the character string
     * \return true for separator, false for non separator.
     */
    virtual bool isSentenceSeparator(const char* p) const = 0;
};

} // namespace cma

#endif // CMA_CTYPE_H
