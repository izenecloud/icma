/** \file cma_ctype.cpp
 * \brief CMA_CType gives the character type information.
 * \author Jun Jiang
 * \version 0.1
 * \date Mar 10, 2009
 */

#include "cma_ctype.h"
#include "cma_ctype_gb2312.h"
#include "cma_ctype_big5.h"
#include "cma_ctype_gb18030.h"

#include <assert.h>

using namespace std;

namespace cma
{

CMA_CType* CMA_CType::instance(Knowledge::EncodeType type)
{
   switch(type)
    {
	case Knowledge::ENCODE_TYPE_GB2312:
		return CMA_CType_GB2312::instance();

	case Knowledge::ENCODE_TYPE_BIG5:
		return CMA_CType_Big5::instance();

	case Knowledge::ENCODE_TYPE_GB18030:
		return CMA_CType_GB18030::instance();

	default:
		assert(false && "unknown character encode type");
	    return 0;
    }
}


CMA_CType::~CMA_CType()
{
}

bool CMA_CType::isPunct(const char* p) const{
    return getCharType(p, CHAR_TYPE_INIT, 0) == CHAR_TYPE_PUNC;
}

size_t CMA_CType::length(const char* p) const{
    size_t ret = 0;
    while(p){
        unsigned int len = getByteCount(p);
        //len can be 0
        if(!len)
            return ret;
        p += len;
        ++ret;
    }
    return ret;
}

unsigned int CMA_CType::getEncodeValue(const char* p) const
{
	unsigned int bytes = getByteCount( p );
	if(!bytes)
		return 0;

	const unsigned char* uc = (const unsigned char*)p;

	unsigned int val = 0;
	switch(bytes)
	{
			case 1:
					val = uc[0];
					break;
			case 2:
					val = uc[0] << 8 | uc[1];
					break;
			case 3:
					val =  uc[0] << 16 | uc[1] << 8 | uc[2];
					break;
			case 4:
					val = uc[0] << 24 | uc[1] << 16 | uc[2] << 8 | uc[3] ;
					break;
			default:
					assert(false && "Cannot handle 'Character's length > 4'");
					return 0;
	}

	return val;
}

} // namespace cma
