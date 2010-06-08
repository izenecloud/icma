/** \file cma_ctype_gb2312.cpp
 * \brief CMA_CType_GB2312 gives the character type information in GB2312 encoding.
 * \author Vernkin
 * \version 0.1
 * \date Mar 11, 2009
 */

#include "icma/type/cma_ctype_gb2312.h"

#include <cassert>
#include <cctype>

using namespace std;

namespace cma
{

CMA_CType_GB2312* CMA_CType_GB2312::instance_;

CMA_CType_GB2312* CMA_CType_GB2312::instance()
{
    if(instance_ == 0)
    {
	instance_ = new CMA_CType_GB2312;
    }

    return instance_;
}

CMA_CType_GB2312::CMA_CType_GB2312()
{
	type_ = Knowledge::ENCODE_TYPE_GB2312;
}

unsigned int CMA_CType_GB2312::getByteCount(const char* p) const
{
    assert(p);

    const unsigned char* uc = (const unsigned char*)p;

    if(uc[0] == 0)
        return 0;

    if(uc[0] < 0x80)
        return 1; // encoding in ASCII

    return 2; // encoding in GB2312
}

} // namespace cma
