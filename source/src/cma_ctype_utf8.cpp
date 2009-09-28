/** \file cma_ctype_utf8.cpp
 * \brief CMA_CType_UTF8 gives the character type information in UTF8 encoding.
 * \author Vernkin
 * \version 0.1
 * \date Mar 11, 2009
 */

#include "cma_ctype_utf8.h"

#include <cassert>
#include <cctype>

using namespace std;

namespace cma
{

CMA_CType_UTF8* CMA_CType_UTF8::instance_;

CMA_CType_UTF8* CMA_CType_UTF8::instance()
{
    if(instance_ == 0)
    {
	instance_ = new CMA_CType_UTF8;
    }

    return instance_;
}

CMA_CType_UTF8::CMA_CType_UTF8()
{
}

unsigned int CMA_CType_UTF8::getByteCount(const char* p) const
{
	unsigned char val = (unsigned char)p[0];

	if( val == 0 )        // length   byte1     byte2     byte3    byte4
		return 0;
	else if( val < 0x80 ) //    1    0xxxxxxx
		return 1;
	else if( val < 0xE0 ) //    2    110yyyxx  10xxxxxx
		return 2;
	else if( val < 0xF0)  //    3    1110yyyy  10yyyyxx  10xxxxxx
		return 3;
	return 4;             //    4    11110zzz  10zzyyyy  10yyyyxx  10xxxxxx
}

}
