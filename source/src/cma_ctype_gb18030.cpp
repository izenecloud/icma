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

namespace gb18030type{

    const unsigned char DOT = (unsigned char)('.'); ///< dot

    const unsigned char COMMA = (unsigned char)(','); ///< comma

    const unsigned char HYPHEN = (unsigned char)('-'); ///< hyphen

    const unsigned short HYPHEN_1 = 0xa3ad; ///< －

    const unsigned short ZHI_CH = 0xd6ae; ///< 之

    const unsigned short DIAN_CH = 0xb5e3; ///< 点

    const unsigned short FEN_CH = 0xb7d6; ///< 分

    const unsigned short DUO_CH = 0xb6e0; ///< 多

    const unsigned short YU_CH = 0xd3e0; ///< 余

    const unsigned short CHENG_CH = 0xb3c9; ///< 成

    const unsigned short BAN_CH = 0xb0eb; ///< 半

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

        return false;
    }

    /**
     * Whether it is digit in any cases
     */
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
            case 0xc1bd: //两
                return true;

        }

        return false;
    }

    /**
     * Whether it is space in any cases
     */
    inline bool isAbsSpace(const unsigned char* uc){
        if(uc[0] < 0x80)
            return isspace(uc[0]); // check by std library

        //full-width space in GB18030
        if(uc[0] == 0xa1 && uc[1] == 0xa1)
            return true;

        return false;
    }

    /**
     * Whether it is letter in any cases
     */
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

    /**
     * Whether it is date in any cases
     */
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
    		assert(false && ("Invalid third and fourth bytes of gb18030: " +
    				(int)uc[2] + "," + (int)uc[3] + ",str=" + p));
    		return 1;
    	}
    	else
    	{
    		assert(false && ("Invalid second byte of gb18030: " + (int)uc[1] + ",str=" + p));
    		return 1;
    	}

    }
    assert(false && ("Invalid first byte of gb18030: " + (int)uc[0] + ",str=" + p));
    return 1;
}

CharType CMA_CType_GB18030::getCharType(const char* p, CharType preType,
        const char* nextP) const
{
    using namespace gb18030type;

    assert(p);

    const unsigned char* uc = (const unsigned char*)p;
    const unsigned char* nextUc = (const unsigned char*)nextP;

    unsigned short value = (uc[0] < 0x80) ? uc[0] : uc[0] << 8 | uc[1];

    switch(value){
        case DOT:
            if( nextUc && (preType == CHAR_TYPE_LETTER || preType == CHAR_TYPE_NUMBER)
                    && (isAbsLetter(nextUc) || isAbsDigit(nextUc)) )
                return CHAR_TYPE_LETTER;
            return CHAR_TYPE_PUNC;

        case COMMA:
            if( nextUc && preType == CHAR_TYPE_NUMBER && isAbsDigit(nextUc) )
                return CHAR_TYPE_LETTER;
            return CHAR_TYPE_PUNC;

        case HYPHEN:
        case HYPHEN_1:
           if( nextUc && (preType == CHAR_TYPE_LETTER || preType == CHAR_TYPE_NUMBER)
                    && (isAbsLetter(nextUc) || isAbsDigit(nextUc)) )
                return CHAR_TYPE_LETTER;
            return CHAR_TYPE_PUNC;

        case ZHI_CH:
        case DIAN_CH:
        case CHENG_CH:
        case BAN_CH:
            if( nextUc && preType == CHAR_TYPE_NUMBER && isAbsDigit(nextUc) )
                return CHAR_TYPE_NUMBER;
            break;

        case FEN_CH:
            // x分之x
            if(nextUc && nextUc[0] >= 0x80){
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
        return CHAR_TYPE_PUNC; //return punctuation here

    //other constraint to check day?
    if(isAbsDate(uc)){
        if(preType == CHAR_TYPE_NUMBER)
            return CHAR_TYPE_DATE;
        return CHAR_TYPE_OTHER;
    }

    return CHAR_TYPE_OTHER;
}

bool CMA_CType_GB18030::isSpace(const char* p) const
{
    assert(p);

    const unsigned char* uc = (const unsigned char*)p;

    if(uc[0] < 0x80)
        return isspace(uc[0]); // check by std library

    //full-width space in GB18030
    if(uc[0] == 0xa1 && uc[1] == 0xa1)
        return true;

    return false;
}

bool CMA_CType_GB18030::isSentenceSeparator(const char* p) const
{
    assert(p);

    const unsigned char* uc = (const unsigned char*)p;

    if(uc[0] < 0x80)
    {
        switch(*p)
        {
            case '?':
            case '!':
                return true;

            default:
                return false;
        }
    }

    unsigned short value = uc[0] << 8 | uc[1];
    //。！？；：＂
    switch(value)
    {
        case 0xa1a3:
        case 0xa3a1:
        case 0xa3bf:
        case 0xa3bb:
        case 0xa3ba:
            return true;
    }

    return false;
}

} // namespace cma
