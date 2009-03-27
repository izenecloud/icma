/** \file analyzer.cpp
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Feb 17, 2009
 */

#include "analyzer.h"

#include <cassert>

namespace cma
{

Analyzer::Analyzer()
    : options_(OPTION_TYPE_NUM)
{
    options_[OPTION_TYPE_POS_TAGGING] = 1; // print part-of-speech tags defaultly
    options_[N_BEST] = 1;
}

Analyzer::~Analyzer()
{
}

void Analyzer::setOption(OptionType nOption, double nValue)
{
    options_[nOption] = nValue;
}

double Analyzer::getOption(OptionType nOption) const
{
    return options_[nOption];
}

} // namespace cma
