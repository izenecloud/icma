/** \file cma_ctype_gb2312.cpp
 * 
 * \author Vernkin
 * \version 0.1
 * \date Mar 11, 2009
 */

#include "cma_ctype_gb2312.h"

#include <cassert>
#include <cctype>

using namespace std;

namespace cma
{

namespace gb2312type{

    const unsigned char DOT = (unsigned char)('.');

    const unsigned char COMMA = (unsigned char)(',');

    const unsigned char HYPHEN = (unsigned char)('-');

    const unsigned short ZHI_CH = 0xd6ae; //之  

    const unsigned short DIAN_CH = 0xb5e3; //点

    const unsigned short FEN_CH = 0xb7d6; //分

    const unsigned short DUO_CH = 0xb6e0; //多

    const unsigned short YU_CH = 0xd3e0; //余

    /**
     * Whether it is punctuation in any cases
     */
    inline bool isAbsPunt(const unsigned char* uc){

        if(uc[0] == DOT || uc[0] == COMMA)
            return false;

        if(uc[0] < 0x80)
            return ispunct(uc[0]); // check by std library

        //、。
        if(uc[0] == 0xa1 &&
                ((uc[1] >= 0xa2 && uc[1] <= 0xef)
                 || (uc[1] >= 0xf1 && uc[1] <= 0xfe)))
            return true;

        //！＂：；［｛
        if(uc[0] == 0xa3 &&
                ((uc[1] >= 0xa1 && uc[1] <= 0xaf)
                 || (uc[1] >= 0xba && uc[1] <= 0xc0)
                 || (uc[1] >= 0xdb && uc[1] <= 0xe0)
                 || (uc[1] >= 0xfb && uc[1] <= 0xfe)))
            return true;
        /*
        unsigned short value = uc[0] << 8 | uc[1];
        //。！？；：
        switch(value)
        {
            case 0xa1a3:
            case 0xa3a1:
            case 0xa3bf:
            case 0xa3bb:
            case 0xa3ba:
                return true;
        }
        */

        return false;
    }

    inline bool isAbsDigit(const unsigned char* uc){
        if((uc[0] >= 0x30 && uc[0] <= 0x39) //0123456789
            || (uc[0] == 0x25) //%
            || (uc[0] == 0xa3 && uc[1] >= 0xb0 && uc[1] <= 0xb9)) //０１２３４５６７８９
            return true;

        if(uc[0] < 0x80)
            return false;

        unsigned short value = uc[0] << 8 | uc[1];
        switch(value)
        {
            case 0xa1f0: //○
            case 0xa3ae: //．
            case 0xa3a5: //％
            case 0xc1e3: case 0xd2bb: case 0xb6fe: case 0xc8fd: case 0xcbc4: //零一二三四
            case 0xcee5: case 0xc1f9: case 0xc6df: case 0xb0cb: case 0xbec5: //五六七八九
            case 0xcaae: case 0xb0d9: case 0xc7a7: case 0xcdf2: case 0xd2da: //十百千万亿
                return true;

        }

        return false;
    }

    inline bool isAbsSpace(const unsigned char* uc){
        if(uc[0] < 0x80)
            return isspace(uc[0]); // check by std library

        //full-width space in GB2312
        if(uc[0] == 0xa1 && uc[1] == 0xa1)
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
            if(uc[0] == 0xa3 &&
                    ((uc[1] >= 0xc1 && uc[1] <= 0xda) //Ａ-Ｚ
                     || (uc[1] >= 0xe1 && uc[1] <= 0xfa))) //ａ-ｚ
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
            case 0xc4ea: case 0xd4c2: case 0xc8d5: //年月日
            case 0xcab1: case 0xb7d6: case 0xc3eb: //时分秒
            case DIAN_CH: //点
                return true;
        }
        return false;
    }
}

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

CharType CMA_CType_GB2312::getCharType(const char* p, CharType preType,
        const char* nextP) const
{
    using namespace gb2312type;

    assert(p);

    const unsigned char* uc = (const unsigned char*)p;
    const unsigned char* nextUc = (const unsigned char*)nextP;

    unsigned short value = (uc[0] < 0x80) ? uc[0] : uc[0] << 8 | uc[1];

    switch(value){
        case DOT:
            if(nextUc == 0)
                return CHAR_TYPE_PUNC;
            if( (preType == CHAR_TYPE_LETTER || preType == CHAR_TYPE_NUMBER)
                    && (isAbsLetter(nextUc) || isAbsDigit(nextUc)) )
                return CHAR_TYPE_LETTER;
            return CHAR_TYPE_PUNC;

        case COMMA:
            if(nextUc == 0)
                return CHAR_TYPE_PUNC;
            if( preType == CHAR_TYPE_NUMBER && isAbsDigit(nextUc) )
                return CHAR_TYPE_LETTER;
            return CHAR_TYPE_PUNC;

        case HYPHEN:
            if( (preType == CHAR_TYPE_LETTER || preType == CHAR_TYPE_NUMBER)
                    && (isAbsLetter(nextUc) || isAbsDigit(nextUc)) )
                return CHAR_TYPE_LETTER;
            return CHAR_TYPE_PUNC;

        case ZHI_CH:
        case DIAN_CH:
            if(nextUc == 0)
                break;
            if( preType == CHAR_TYPE_NUMBER && isAbsDigit(nextUc) )
                return CHAR_TYPE_NUMBER;
            break;

        case FEN_CH:
            // x分之x
            if(nextUc != 0 && nextUc[0] >= 0x80){
                unsigned short nextValue = nextUc[0] << 8 | nextUc[1];
                if(nextValue == ZHI_CH)
                    return CHAR_TYPE_NUMBER;
            }
            break;

        case DUO_CH:
        case YU_CH:
            if(preType == CHAR_TYPE_NUMBER)
                return CHAR_TYPE_NUMBER;
            break;
    }


    //check common type
    if(isAbsLetter(uc))
        return CHAR_TYPE_LETTER;

    if(isAbsDigit(uc))
        return CHAR_TYPE_NUMBER;

    if(isAbsPunt(uc))
        return CHAR_TYPE_PUNC;

    if(isAbsSpace(uc))
        return CHAR_TYPE_SPACE;

    //other constraint to check day?
    if(isAbsDate(uc)){
        if(preType == CHAR_TYPE_NUMBER)
            return CHAR_TYPE_DATE;
        return CHAR_TYPE_OTHER;
    }

    return CHAR_TYPE_OTHER;
}



} // namespace cma
