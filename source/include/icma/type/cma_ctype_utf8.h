/*
 * cma_ctype_utf8.h
 *
 *  Created on: Sep 28, 2009
 *      Author: vernkin
 */

#ifndef CMA_CTYPE_UTF8_H_
#define CMA_CTYPE_UTF8_H_

#include "icma/type/cma_ctype.h"

namespace cma
{

/**
 * \brief CMA_CType_UTF8 gives the character type information in UTF8 encoding.
 *
 */
class CMA_CType_UTF8 : public CMA_CType
{
public:
    /**
     * Create an instance of \e CMA_CType_UTF8.
     * \return the pointer to instance
     */
    static CMA_CType_UTF8* instance();

    /**
     * Get the byte count of the first character pointed by \e p, which character is in a specific encoding.
     * \param p pointer to the character string
     * \return the count of bytes.
     */
    virtual unsigned int getByteCount(const char* p) const;


protected:
    /**
     * Avoid invoked directly
     */
    CMA_CType_UTF8();

private:
    /** the instance of \e CMA_CType_UTF8 */
    static CMA_CType_UTF8* instance_;
};

} // namespace cma

#endif /* CMA_CTYPE_UTF8_H_ */
