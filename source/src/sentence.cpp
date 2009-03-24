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

std::vector<std::string> Sentence::posTable_;

void Sentence::setString(const char* pString)
{
    raw_ = pString;
    candidates_.clear();
    scores_.clear();
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
    if(posIdx == -1)
	return 0;

    return posTable_[posIdx].c_str();
}

double Sentence::getScore(int nPos) const
{
    return scores_[nPos];
}

int Sentence::getOneBestIndex(void) const
{
    if(scores_.empty())
	return -1;

    assert(scores_.size() > 0 && scores_.size() == candidates_.size());

    return max_element(scores_.begin(), scores_.end()) - scores_.begin();
}

void Sentence::addList(const MorphemeList& morphemeList, double score)
{
    candidates_.push_back(morphemeList);
    scores_.push_back(score);
}

void Sentence::setPOSTable(const std::vector<std::string>& table)
{
    posTable_ = table;
}

} // namespace cma
