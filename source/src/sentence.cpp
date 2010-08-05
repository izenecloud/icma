/** \file sentence.cpp
 * \brief Sentence saves the results of Chinese morphological analysis.
 * \author Jun Jiang
 * \version 0.1
 * \date Feb 17, 2009
 */

#include "sentence.h"
#include "icma/pos_table.h"

#include <icma/util/StringArray.h>

#include <algorithm>
#include <cassert>

namespace cma
{

Morpheme::Morpheme()
    : posCode_(-1)
{
}

Sentence::Sentence()
{
	//do nothing
}

Sentence::Sentence(const char* pString) : raw_( pString )
{
}

void Sentence::setString(const char* pString)
{
    raw_ = pString;
    candidates_.clear();
    segment_.clear();
    pos_.clear();
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
    return (segment_[nPos].first)[nIdx];
}

bool Sentence::isIndexWord(int nPos, int nIdx) const
{
    return candidates_[nPos][nIdx].isIndexed;
}

int Sentence::getPOS(int nPos, int nIdx) const
{
    return candidates_[nPos][nIdx].posCode_;
}

const char* Sentence::getStrPOS(int nPos, int nIdx) const
{
    return pos_[nPos][nIdx];
}

MorphemeList* Sentence::getMorphemeList(int nPos)
{
	return &candidates_[nPos];
}

double Sentence::getScore(int nPos) const
{
    return segment_[nPos].second;
}

void Sentence::setScore(int nPos, double nScore)
{
    segment_[nPos].second = nScore;
}

int Sentence::getOneBestIndex(void) const
{
    if( segment_.empty() )
	   return -1;
    if( segment_.size() == 1 )
       return 0;

    double bestScore = segment_[0].second;
    int bestIdx = 0;
    int size = segment_.size();
    for( int i = 1; i < size; ++i )
    {
        double tmpScore = segment_[ i ].second;
        if( tmpScore > bestScore )
        {
            bestScore = tmpScore;
            bestIdx = i;
        }

    }

    return bestIdx;
}

void Sentence::addList( const MorphemeList& morphemeList )
{
    candidates_.push_back(morphemeList);
}

} // namespace cma
