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
    string enc(encType);
	if(enc == "gb2312" || enc == "GB2312")
    {
        return Knowledge::ENCODE_TYPE_GB2312;
    }
    else if(enc == "big5" || enc == "BIG5")
    {
        return Knowledge::ENCODE_TYPE_BIG5;
    }
    else if(enc == "gb18030" || enc == "GB18030")
    {
        return Knowledge::ENCODE_TYPE_GB18030;
    }

    assert(false && "unknown character encode type");
    return Knowledge::ENCODE_TYPE_GB18030;
}

} // namespace cma
