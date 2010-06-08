/** \file cma_ctype_gb2312.h
 * \brief CMA_CType_GB2312 gives the character type information in GB2312 encoding.
 * \author Vernkin
 * \version 0.2
 * \date Mar 11, 2009
 */

#ifndef CMA_CTYPE_GB2312_H
#define CMA_CTYPE_GB2312_H

#include "icma/type/cma_ctype.h"

namespace cma
{

/**
 * \brief CMA_CType_GB2312 gives the character type information in GB2312 encoding.
 *
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
     * Get the character type.
     * \param p pointer to the string to be checked
     * \param preType the chartype of the previous character
     * \param nextP the pointer of the next character, it can be 0
     * \return the character type.
     */
    //virtual CharType getCharType(const char* p, CharType preType,
     //       const char* nextP) const;

    /**
     * Check whether is white-space character.
     * White-space characters are " \t\n\v\f\r", and also space character in specific encoding.
     * \param p pointer to the character string
     * \return true for white-space character, false for non white-space character.
     */
    //virtual bool isSpace(const char* p) const;

    /**
     * Check whether is a separator of sentence.
     * \param p pointer to the character string
     * \return true for separator, false for non separator.
     */
    //virtual bool isSentenceSeparator(const char* p) const;

protected:
    /**
     * Avoid invoked directly
     */
    CMA_CType_GB2312();

private:
    /** the instance of \e CMA_CType_GB2312 */
    static CMA_CType_GB2312* instance_;
};

} // namespace cma

#endif // CMA_CTYPE_GB2312_H
