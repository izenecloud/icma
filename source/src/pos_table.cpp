/** \file pos_table.cpp
 * \brief POSTable stores the global table of part-of-speech tags
 * \author Jun Jiang
 * \version 0.1
 * \date Apr 02, 2009
 */

#include "pos_table.h"

#include <cassert>
#include <fstream>

using namespace std;

const char* EMPTY = "";

namespace cma
{

bool Nocase::operator() (const std::string& x, const std::string& y) const
{
    std::string::const_iterator p = x.begin();
    std::string::const_iterator q = y.begin();

    while(p != x.end() && q != y.end() && toupper(*p) == toupper(*q))
    {
		++p;
		++q;
    }

    if(p == x.end())
    	return q != y.end();

    if(q == y.end())
    	return false;

    return toupper(*p) < toupper(*q);
}

POSTable::POSTable()
{
}

int POSTable::addPOS(const std::string& pos)
{
    // check whether has been added before
    POSMap::const_iterator iter = posMap_.find(pos);
    if(iter != posMap_.end())
    {
        return iter->second;
    }

    int index = posTable_.size();
    posMap_[pos] = index;
    posTable_.push_back( pair<string, bool>(pos, true));

    return index;
}

int POSTable::getCodeFromStr(const std::string& pos) const
{
    POSMap::const_iterator iter = posMap_.find(pos);
    if(iter != posMap_.end())
    	return iter->second;
    else
    	return -1;
}

const char* POSTable::getStrFromCode(int index) const
{
    if(index < 0 || static_cast<size_t>(index) >= posTable_.size())
    	return EMPTY;

    return posTable_[index].first.c_str();
}

int POSTable::size() const
{
    assert(posTable_.size() == posMap_.size() && "POS table size should equal to POS map size");
    return posTable_.size();
}

int POSTable::getCodeFromType(POSType type) const
{
    assert(type >= 0 && static_cast<unsigned int>(type) < typeTable_.size());

    return typeTable_[type];
}

bool POSTable::isIndexPOS( int posCode ) const
{
	// Error POS Index return true by default
	if(posCode < 0 || static_cast<size_t>(posCode) >= posTable_.size())
		return true;
	return posTable_[ posCode ].second;
}

bool POSTable::setIndexPOS( int posCode, bool isIndex )
{
	if(posCode < 0 || static_cast<size_t>(posCode) >= posTable_.size())
			return false;
	return posTable_[ posCode ].second = isIndex;
}

void POSTable::resetIndexPOSList( bool defVal )
{
	for( POSTableInfo::iterator itr = posTable_.begin();
			itr != posTable_.end(); ++itr )
		itr->second = defVal;
}

int POSTable::setIndexPOSList( vector<string>& posList )
{
	int ret = 0;
	for( vector<string>::iterator itr = posList.begin();
			itr != posList.end(); ++itr )
	{
		if( setIndexPOS( getCodeFromStr( *itr), true ) )
			++ret;
	}
	return ret;
}

} // namespace cma

