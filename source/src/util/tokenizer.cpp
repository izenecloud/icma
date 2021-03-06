/** \file tokenizer.cpp
 * \brief Tokenizer tokenizes a raw input string in specific encoding to a sequence of characters.
 * \author Jun Jiang
 * \version 0.1
 * \date Mar 16, 2009
 */

#include "icma/util/tokenizer.h"

#include <cstring>
#include <cassert>

using namespace std;

namespace cma
{

CTypeTokenizer::CTypeTokenizer(const CMA_CType* ctype)
    : ctype_(ctype), raw_(0)
{
    // initialize character buffer with 0
    memset(buffer_, 0, sizeof(buffer_));
}

CTypeTokenizer::CTypeTokenizer(const CMA_CType* ctype, const char* str)
    : ctype_(ctype), raw_(str)
{
    // initialize character buffer with 0
    memset(buffer_, 0, sizeof(buffer_));
}

void CTypeTokenizer::assign(const char* str)
{
    assert(str);

    raw_ = str;
}

const char* CTypeTokenizer::next()
{
    if(raw_ == 0 || *raw_ == 0)
        return 0;
    unsigned int bc = ctype_->getByteCount(raw_);
    //assert(bc > 0 && bc < BUFFER_SIZE);

    memcpy( buffer_, raw_, bc );

    // append 0 as end of string
    buffer_[bc] = 0;

    // move to next character
    raw_ += bc;
    return buffer_;
}

} // namespace cma
