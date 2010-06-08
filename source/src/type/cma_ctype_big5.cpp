/** \file cma_ctype_big5.cpp
 * \brief CMA_CType_Big5 gives the character type information in Big5 encoding.
 * \author Vernkin
 * \version 0.2
 * \date April 06, 2009
 */

#include "icma/type/cma_ctype_big5.h"

#include <cassert>
#include <cctype>

using namespace std;

namespace cma
{

CMA_CType_Big5* CMA_CType_Big5::instance_;

CMA_CType_Big5* CMA_CType_Big5::instance()
{
    if(instance_ == 0)
    {
	instance_ = new CMA_CType_Big5;
    }

    return instance_;
}

CMA_CType_Big5::CMA_CType_Big5()
{
	type_ = Knowledge::ENCODE_TYPE_BIG5;
}

unsigned int CMA_CType_Big5::getByteCount(const char* p) const
{
    assert(p);

    const unsigned char* uc = (const unsigned char*)p;

    if(uc[0] == 0)
        return 0;

    if(uc[0] < 0x80)
        return 1; // encoding in ASCII

    return 2; // encoding in Big5
}

} // namespace cma
