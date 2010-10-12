/** \file analyzer.cpp
 * \brief Analyzer executes the Chinese morphological analysis.
 * \author Jun Jiang
 * \version 0.1
 * \date Feb 17, 2009
 */

#include "icma/analyzer.h"

#include <cassert>

namespace
{
/** the default delimiter between word and POS tag in the output result of \e runWithString() and \e runWithStream() */
const char* DEFAULT_POS_DELIMITER = "/";

/** the default delimiter between the pairs (word and POS tag) in the output result of \e runWithString() and \e runWithStream() */
const char* DEFAULT_WORD_DELIMITER = "  ";

/** the default value of the sentence delimiter */
const char* DEFAULT_SENTENCE_DEMIMITER = "";

}

namespace cma
{

Analyzer::Analyzer()
    : options_(OPTION_TYPE_NUM),
    posDelimiter_(DEFAULT_POS_DELIMITER),
    wordDelimiter_(DEFAULT_WORD_DELIMITER),
    sentenceDelimiter_(DEFAULT_SENTENCE_DEMIMITER)
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

void Analyzer::setPOSDelimiter(const char* delimiter)
{
    posDelimiter_ = delimiter;
}

const char* Analyzer::getPOSDelimiter() const
{
    return posDelimiter_;
}

void Analyzer::setWordDelimiter(const char* delimiter)
{
    wordDelimiter_ = delimiter;
}

const char* Analyzer::getWordDelimiter() const
{
    return wordDelimiter_;
}

void Analyzer::setSentenceDelimiter(const char* delimiter)
{
    sentenceDelimiter_ = delimiter;
}

const char* Analyzer::getSentenceDelimiter() const
{
    return sentenceDelimiter_;
}

} // namespace cma
