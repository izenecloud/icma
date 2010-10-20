/** \file sentence.cpp
 * \brief Sentence saves the results of Chinese morphological analysis.
 * \author Jun Jiang
 * \version 0.1
 * \date Feb 17, 2009
 */

#include "icma/sentence.h"
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
    : incrementedWordOffsetB_(true)
{
	//do nothing
}

Sentence::Sentence(const char* pString)
    : raw_( pString ),
      incrementedWordOffsetB_(true)
{
}

void Sentence::setString(const char* pString)
{
    raw_ = pString;
    candidates_.clear();
    segment_.clear();
    pos_.clear();
    candMetas_.clear();
    wordOffset_.clear();
}

const char* Sentence::getString(void) const
{
    return raw_.c_str();
}

int Sentence::getListSize(void) const
{
    return candMetas_.size();
}

int Sentence::getCount(int nPos) const 
{
    int listSize = candMetas_.size();
    return ( nPos + 1 >= listSize ) ?
            ( segment_.size() - candMetas_[ nPos ].segOffset_ ) :
            ( candMetas_[ nPos + 1 ].segOffset_ - candMetas_[ nPos ].segOffset_ );
}

const char* Sentence::getLexicon(int nPos, int nIdx) const
{
    return segment_[ candMetas_[ nPos ].segOffset_ + nIdx ];
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
    return pos_[ candMetas_[ nPos ].posOffset_ + nIdx ];
}

size_t Sentence::getOffset(int nPos, int nIdx) const
{
    if( incrementedWordOffsetB_ == true )
        return nIdx;
    return wordOffset_[ candMetas_[ nPos ].wdOffset_ + nIdx ];
}

MorphemeList* Sentence::getMorphemeList(int nPos)
{
	return &candidates_[nPos];
}

double Sentence::getScore(int nPos) const
{
    return candMetas_[nPos].score_;
}

void Sentence::setScore(int nPos, double nScore)
{
    candMetas_[nPos].score_ = nScore;
}

int Sentence::getOneBestIndex(void) const
{
    if( candMetas_.empty() )
	   return -1;
    int size = candMetas_.size();
    if( size == 1 )
       return 0;

    double bestScore = candMetas_[0].score_;
    int bestIdx = 0;

    for( int i = 1; i < size; ++i )
    {
        double tmpScore = candMetas_[ i ].score_;
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

void Sentence::setIncrementedWordOffset( bool flag )
{
    incrementedWordOffsetB_ = flag;
}

bool Sentence::isIncrementedWordOffset()
{
    return incrementedWordOffsetB_;
}

} // namespace cma
