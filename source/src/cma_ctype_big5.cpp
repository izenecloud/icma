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

namespace big5type{

    const unsigned char DOT = (unsigned char)('.');

    const unsigned char COMMA = (unsigned char)(',');

    const unsigned short ZHI_CH = 0xa4a7; //之

    const unsigned short DIAN_CH = 0xc249; //點

    /**
     * Whether it is punctuation in any cases
     */
    inline bool isAbsPunt(const unsigned char* uc){

        if(uc[0] == DOT || uc[0] == COMMA)
            return false;

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
        /*
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
        */

        return false;
    }

    inline bool isAbsDigit(const unsigned char* uc){
        if((uc[0] >= 0x30 && uc[0] <= 0x39) //0123456789
            || (uc[0] == 0x25) //%
            || (uc[0] == 0xa2 && uc[1] >= 0xaf && uc[1] <= 0xb8)) //０１２３４５６７８９
            return true;

        if(uc[0] < 0x80)
            return false;

        unsigned short value = uc[0] << 8 | uc[1];
        switch(value)
        {
            case 0xa1b3: //○
            case 0xa144: //．
            case 0xa248: //％
            case 0xb973: case 0xa440: case 0xa447: case 0xa454: case 0xa57c: //零一二三四
            case 0xa4ad: case 0xa4bb: case 0xa443: case 0xa44b: case 0xa445: //五六七八九
            case 0xa451: case 0xa6ca: case 0xa464: case 0xb855: case 0xbbf5: //十百千萬億
            return true;

        }

        return false;
    }

    inline bool isAbsSpace(const unsigned char* uc){
        if(uc[0] < 0x80)
            return isspace(uc[0]); // check by std library

        //full-width space in Big5
        if(uc[0] == 0xa1 && uc[1] == 0x40)
            return true;

        return false;
    }

    inline bool isAbsLetter(const unsigned char* uc){
        if(uc[0] < 0x80)
        {
            if(isalpha(uc[0])) //A-Za-z
                return true; // check letter by std library
        }
        else
        {
            if((uc[0] == 0xa2 && (uc[1] >= 0xcf && uc[1] <= 0xfe)) //Ａ-Ｚａ-ｖ
                    || (uc[0] == 0xa3 && (uc[1] >= 0x40 && uc[1] <= 0x43))) //ｗ-ｚ
                return true;
        }

        return false;
    }

    inline bool isAbsDate(const unsigned char* uc){
        if(uc[0] < 0x80)
            return false;
        unsigned short value = uc[0] << 8 | uc[1];
        switch(value)
        {
            case 0xa67e: case 0xa4eb: case 0xa4e9: //年月日
            case 0xaec9: case 0xa4c0: case 0xaced: //時分秒
            case DIAN_CH:
                return true;
        }
        return false;
    }
}

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

CharType CMA_CType_Big5::getCharType(const char* p, CharType preType,
        const char* nextP) const
{
    using namespace big5type;

    assert(p);

    const unsigned char* uc = (const unsigned char*)p;

    if(isAbsLetter(uc))
        return CHAR_TYPE_LETTER;

    if(isAbsDigit(uc))
        return CHAR_TYPE_NUMBER;

    if(isAbsPunt(uc))
        return CHAR_TYPE_PUNC;

    //other constraint to check day?
    if(isAbsDate(uc))
        return CHAR_TYPE_DATE;

    //check special character, if only has one byte

    if(uc[0] < 0x80)
        return CHAR_TYPE_OTHER;

    //if has 2 bytes

    return CHAR_TYPE_OTHER;
}


} // namespace cma
