/** \file knowledge.cpp
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Feb 17, 2009
 */

#include "knowledge.h"

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

} // namespace cma
