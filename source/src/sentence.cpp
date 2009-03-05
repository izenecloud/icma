/** \file sentence.cpp
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Feb 17, 2009
 */

#include "sentence.h"

#include <cassert>

namespace cma
{

/**
 * string of part-of-speech table.
 */
static const char* POS_TABLE[] = {"A", "AD", "N", "P", "V", "W"};

void Sentence::setString(const char* pString)
{
    raw_ = pString;
    candidates_.clear();
}

const char* Sentence::getString(void) const
{
    return raw_.c_str();
}

int Sentence::getListSize(void) const
{
    return candidates_.size();
}

int Sentence::getCount(int nPos) const 
{
    return candidates_[nPos].size();
}

const char* Sentence::getLexicon(int nPos, int nIdx) const
{
    return candidates_[nPos][nIdx].lexicon_.c_str();
}

int Sentence::getPOS(int nPos, int nIdx) const
{
    return candidates_[nPos][nIdx].posCode_;
}

const char* Sentence::getStrPOS(int nPos, int nIdx) const
{
    int posIdx = getPOS(nPos, nIdx);
    return POS_TABLE[posIdx];
}

double Sentence::getScore(int nPos) const
{
    return scores_[nPos];
}

int Sentence::getOneBestIndex(void) const
{
    assert(scores_.size() > 0 && scores_.size() == candidates_.size());

    return max_element(scores_.begin(), scores_.end()) - scores_.begin();
}

void Sentence::addList(const MorphemeList& morphemeList, double score)
{
    candidates_.push_back(morphemeList);
    scores_.push_back(score);
}

} // namespace cma
