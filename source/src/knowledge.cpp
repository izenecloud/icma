/** \file knowledge.cpp
 * \brief Knowledge manages the linguistic information for Chinese morphological analysis.
 * \author Jun Jiang
 * \version 0.1
 * \date Feb 17, 2009
 */

#include "knowledge.h"
#include <string>
#include <cassert>

using std::string;

namespace cma
{

/**
 * Get a string in lower alphabets.
 * \param s the original string
 * \return the converted string with lower alphabets. For example, "gb2312" is returned when "GB2312" is input.
 */
string toLower(const char* s) {
    string str(s);
    for(size_t i=0; i<str.size(); ++i)
    {
        char c = str[i];
        if((c >= 'A') && (c <= 'Z'))
        {
            c += 'a' - 'A';
            str[i] = c;
        }
    }
    return str;
}

Knowledge::Knowledge()
    : encodeType_(ENCODE_TYPE_GB2312)
{
}

Knowledge::~Knowledge()
{
}

void Knowledge::setEncodeType(EncodeType type)
{
    encodeType_ = type;
}

Knowledge::EncodeType Knowledge::getEncodeType() const
{
    return encodeType_;
}

Knowledge::EncodeType Knowledge::decodeEncodeType(const char* encType){
    string enc = toLower(encType);
	if(enc == "gb2312" || enc == "gb 2312")
    {
        return Knowledge::ENCODE_TYPE_GB2312;
    }
    else if(enc == "big5" || enc == "big 5")
    {
        return Knowledge::ENCODE_TYPE_BIG5;
    }
    else if(enc == "gb18030" || enc == "gb 18030")
    {
        return Knowledge::ENCODE_TYPE_GB18030;
    }
    else if(enc == "utf8" || enc == "utf-8")
	{
		return Knowledge::ENCODE_TYPE_UTF8;
	}
#ifdef USE_UTF_16
    else if(enc == "utf16" || enc == "utf-16" || enc == "ucs2" || enc == "ucs-2")
	{
		return Knowledge::ENCODE_TYPE_UTF16;
	}
#endif
    assert(false && "unknown character encode type");
    return Knowledge::ENCODE_TYPE_NUM;
}

} // namespace cma
