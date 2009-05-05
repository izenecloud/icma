/** \file cma_ctype_big5.h
 * \brief CMA_CType_Big5 gives the character type information in Big5 encoding.
 * \author Vernkin
 * \version 0.2
 * \date April 06, 2009
 */

#ifndef CMA_CTYPE_BIG5_H
#define CMA_CTYPE_BIG5_H

#include "cma_ctype.h"

namespace cma
{

/**
 * \brief CMA_CType_Big5 gives the character type information in Big5 encoding.
 *
 */
class CMA_CType_Big5 : public CMA_CType
{
public:
    /**
     * Create an instance of \e CMA_CType_Big5.
     * \return the pointer to instance
     */
    static CMA_CType_Big5* instance();

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
    virtual CharType getCharType(const char* p, CharType preType,
            const char* nextP) const;

protected:
    /**
     * Avoid invoked directly
     */
    CMA_CType_Big5();

private:
    /** the instance of \e CMA_CType_Big5 */
    static CMA_CType_Big5* instance_;
};

} // namespace cma

#endif // CMA_CTYPE_BIG5_H
