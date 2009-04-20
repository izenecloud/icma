/** \file cma_ctype_big5.cpp
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date April 06, 2009
 */

#include "cma_ctype_big5.h"

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

bool CMA_CType_Big5::isPunct(const char* p) const
{
    assert(p);

    const unsigned char* uc = (const unsigned char*)p;

    if(uc[0] < 0x80)
        return ispunct(uc[0]); // check by std library

    //，、。
    if(uc[0] == 0xa1 &&
            ((uc[1] >= 0x41 && uc[1] <= 0x7e)
             || (uc[1] >= 0xa1 && uc[1] <= 0xb2)
             || (uc[1] >= 0xb4 && uc[1] <= 0xfe)))
        return true;

    //＼∕﹨＄￥〒
    if(uc[0] == 0xa2 &&
            ((uc[1] >= 0x40 && uc[1] <= 0x7e)
             || (uc[1] >= 0xa1 && uc[1] <= 0xae)))
        return true;

    return false;
}

CharType CMA_CType_Big5::getCharType(const char* p) const
{
    assert(p);

    const unsigned char* uc = (const unsigned char*)p;

    if((uc[0] >= 0x30 && uc[0] <= 0x39) //0123456789
            || (uc[0] == 0x2e) //.
            || (uc[0] == 0x25) //%
            || (uc[0] == 0xa2 && uc[1] >= 0xaf && uc[1] <= 0xb8)) //０１２３４５６７８９
        return CHAR_TYPE_NUMBER;

    unsigned short value = uc[0] << 8 | uc[1];
    switch(value)
    {
        case 0xa1b3: //○
        case 0xa144: //．
        case 0xa248: //％
        case 0xb973: case 0xa440: case 0xa447: case 0xa454: case 0xa57c: //零一二三四
        case 0xa4ad: case 0xa4bb: case 0xa443: case 0xa44b: case 0xa445: //五六七八九
        case 0xa451: case 0xa6ca: case 0xa464: case 0xb855: case 0xbbf5: //十百千萬億
            return CHAR_TYPE_NUMBER;

        case 0xa67e: case 0xa4eb: case 0xa4e9: //年月日
        case 0xaec9: case 0xa4c0: case 0xaced: //時分秒
            return CHAR_TYPE_DATE;
    }

    if(uc[0] < 0x80)
    {
        if(isalpha(uc[0])) //A-Za-z
            return CHAR_TYPE_LETTER; // check letter by std library
    }
    else
    {
        if((uc[0] == 0xa2 && (uc[1] >= 0xcf && uc[1] <= 0xfe)) //Ａ-Ｚａ-ｖ
		|| (uc[0] == 0xa3 && (uc[1] >= 0x40 && uc[1] <= 0x43))) //ｗ-ｚ
            return CHAR_TYPE_LETTER;
    }

    return CHAR_TYPE_OTHER;
}

bool CMA_CType_Big5::isSpace(const char* p) const
{
    assert(p);

    const unsigned char* uc = (const unsigned char*)p;

    if(uc[0] < 0x80)
        return isspace(uc[0]); // check by std library

    //full-width space in Big5
    if(uc[0] == 0xa1 && uc[1] == 0x40)
        return true;

    return false;
}

bool CMA_CType_Big5::isSentenceSeparator(const char* p) const
{
    assert(p);

    const unsigned char* uc = (const unsigned char*)p;

    if(uc[0] < 0x80)
    {
        switch(*p)
        {
            case '?':
            case '!':
            case ':':
                return true;

            default:
                return false;
        }
    }

    unsigned short value = uc[0] << 8 | uc[1];
    //。！？；：
    switch(value)
    {
        case 0xa143:
        case 0xa149:
        case 0xa148:
        case 0xa146:
        case 0xa147: 
            return true;
    }

    return false;
}

} // namespace cma
