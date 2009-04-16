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
    options_[OPTION_TYPE_POS_TAGGING] = 1; // tag part-of-speech tags defaultly
    options_[OPTION_TYPE_NBEST] = 1; // set the default number of candidate results of runWithSentence()
}

Analyzer::~Analyzer()
{
}

void Analyzer::setOption(OptionType nOption, double nValue)
{
    // check nbest value range
    if(nOption == OPTION_TYPE_NBEST && nValue < 1)
    {
	return;
    }

    options_[nOption] = nValue;
}

double Analyzer::getOption(OptionType nOption) const
{
    return options_[nOption];
}

} // namespace cma
