/** \file tokenizer.cpp
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Mar 16, 2009
 */

#include "tokenizer.h"
#include "cma_ctype.h"

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
    assert(bc > 0 && bc < BUFFER_SIZE);

    for(unsigned int i=0; i<bc; ++i)
        buffer_[i] = *(raw_+i);

    // append 0 as end of string
    buffer_[bc] = 0;

    // move to next character
    raw_ += bc;
    return buffer_;
}

} // namespace cma
