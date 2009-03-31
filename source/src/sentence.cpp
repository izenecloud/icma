/** \file sentence.cpp
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Feb 17, 2009
 */

#include "sentence.h"

#include <cassert>
#include <bits/stl_map.h>

namespace cma
{

std::vector<std::string> Sentence::posTable_;

std::map<std::string, int> Sentence::posMap_;

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

    //return max_element(scores_.begin(), scores_.end()) - scores_.begin();
	return 0;
}

void Sentence::addList(const MorphemeList& morphemeList, double score)
{
    candidates_.push_back(morphemeList);
    scores_.push_back(score);
}

void Sentence::setPOSTable(const std::vector<std::string>& table)
{
    posTable_ = table;
    posMap_.clear();
    int n = (int)table.size();
    for(int i=0; i<n; ++i){
        posMap_[table[i]] = i;
    }
}

int Sentence::getPOSCode(const std::string& strPOS){
    std::map<std::string, int>::iterator itr = posMap_.find(strPOS);
    if(itr == posMap_.end())
        return -1;
    return itr->second;
}

} // namespace cma
