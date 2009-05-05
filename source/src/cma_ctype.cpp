/** \file cma_ctype.cpp
 * \brief CMA_CType gives the character type information.
 * \author Jun Jiang
 * \version 0.1
 * \date Mar 10, 2009
 */

#include "cma_ctype.h"
#include "cma_ctype_gb2312.h"
#include "cma_ctype_big5.h"

#include <cassert>

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

	default:
	    assert(false && "unkown character encode type");
	    return 0;
    }
}

Knowledge::EncodeType CMA_CType::getEncType(string encType){
    if(encType == "gb2312" || encType == "GB2312"){
        return Knowledge::ENCODE_TYPE_GB2312;
    }else if(encType == "big5" || encType == "BIG5"){
        return Knowledge::ENCODE_TYPE_BIG5;
    }

    assert(false && "unkown character encode type");
    return Knowledge::ENCODE_TYPE_NUM;
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

} // namespace cma
