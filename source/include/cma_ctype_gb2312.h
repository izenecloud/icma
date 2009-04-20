/** \file cma_ctype_gb2312.h
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Mar 11, 2009
 */

#ifndef CMA_CTYPE_GB2312_H
#define CMA_CTYPE_GB2312_H

#include "cma_ctype.h"

namespace cma
{

/**
 * CMA_CType_GB2312 gives the character type information in GB2312 encoding.
 */
class CMA_CType_GB2312 : public CMA_CType
{
public:
    /**
     * Create an instance of \e CMA_CType_GB2312.
     * \return the pointer to instance
     */
    static CMA_CType_GB2312* instance();

    /**
     * Get the byte count of the first character pointed by \e p, which character is in a specific encoding.
     * \param p pointer to the character string
     * \return the count of bytes.
     */
    virtual unsigned int getByteCount(const char* p) const;

    /**
     * Check whether is punctuation.
     * \param p pointer to the character string
     * \return true for punctuation, false for non punctuation.
     */
    virtual bool isPunct(const char* p) const;

    /**
     * Get the character type.
     * \param p pointer to the string to be checked
     * \return the character type.
     */
    virtual CharType getCharType(const char* p) const;

    /**
     * Check whether is white-space character.
     * White-space characters are " \t\n\v\f\r", and also space character in specific encoding.
     * \param p pointer to the character string
     * \return true for white-space character, false for non white-space character.
     */
    virtual bool isSpace(const char* p) const;

    /**
     * Check whether is a separator of sentence.
     * \param p pointer to the character string
     * \return true for separator, false for non separator.
     */
    virtual bool isSentenceSeparator(const char* p) const;

protected:
    CMA_CType_GB2312();

private:
    /** the instance of \e CMA_CType_GB2312 */
    static CMA_CType_GB2312* instance_;
};

} // namespace cma

#endif // CMA_CTYPE_GB2312_H
