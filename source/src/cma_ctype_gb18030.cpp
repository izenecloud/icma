/** \file cma_ctype_gb18030.cpp
 * \brief CMA_CType_GB18030 gives the character type information in GB18030 encoding.
 * \author Vernkin
 * \version 0.1
 * \date Mar 11, 2009
 */

#include "cma_ctype_gb18030.h"

#include <cassert>
#include <cctype>

using namespace std;

namespace cma
{

CMA_CType_GB18030* CMA_CType_GB18030::instance_;

CMA_CType_GB18030* CMA_CType_GB18030::instance()
{
    if(instance_ == 0)
    {
	instance_ = new CMA_CType_GB18030;
    }

    return instance_;
}

CMA_CType_GB18030::CMA_CType_GB18030()
{
	type_ = Knowledge::ENCODE_TYPE_GB18030;
}


unsigned int CMA_CType_GB18030::getByteCount(const char* p) const
{
    assert(p);

    const unsigned char* uc = (const unsigned char*)p;

    if(uc[0] == 0)
        return 0;

    if(uc[0] <= 0x80) //0x80 is valid but unsigned
        return 1; // encoding in ASCII

    if(uc[0] <= 0xfe)
    {
    	if(uc[1] >= 0x40 && uc[1] <= 0xfe && uc[1] != 0x7f)
    		return 2;
    	else if(uc[1] >= 0x30 && uc[1] <= 0x39)
    	{
    		if(uc[2] >= 0x81 && uc[2] <= 0xfe && uc[3] >= 0x30 && uc[3] <= 0x39)
    			return 4;
    		//assert(false && ("Invalid third and fourth bytes of gb18030: " +
    		//		(int)uc[2] + "," + (int)uc[3] + ",str=" + p));
    		return 1;
    	}
    	else
    	{
    		//assert(false && ("Invalid second byte of gb18030: " + (int)uc[1] + ",str=" + p));
    		return 1;
    	}

    }
    //assert(false && ("Invalid first byte of gb18030: " + (int)uc[0] + ",str=" + p));
    return 1;
}

} // namespace cma
