/** \file cma_ctype_utf16.cpp
 * \brief CMA_CType_UTF16 gives the character type information in UTF16 encoding.
 * \author Vernkin
 * \version 0.1
 * \date Mar 11, 2009
 */

#include "cma_ctype_utf16.h"

#include <cassert>
#include <cctype>

using namespace std;

namespace cma
{

CMA_CType_UTF16* CMA_CType_UTF16::instance_;

CMA_CType_UTF16* CMA_CType_UTF16::instance()
{
    if(instance_ == 0)
    {
	instance_ = new CMA_CType_UTF16;
    }

    return instance_;
}

CMA_CType_UTF16::CMA_CType_UTF16()
{
}

unsigned int CMA_CType_UTF16::getByteCount(const char* p) const
{
	return 2;
}

}
