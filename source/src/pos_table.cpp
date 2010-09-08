/** \file pos_table.cpp
 * \brief POSTable stores the global table of part-of-speech tags
 * \author Jun Jiang
 * \version 0.1
 * \date Apr 02, 2009
 */

#include "icma/pos_table.h"

#include <cassert>
#include <fstream>
#include <iostream>

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
    posTable_.reserve( 120 );
    indexedFlags_.reserve( 45 );
    posTable_.push_back( "N/A" );
    indexedFlags_.push_back( true );
}

int POSTable::addPOS(const std::string& pos)
{
    const char* str = pos.data();
    // check whether has been added before
    int ret = posMap_.search( str );
    if( ret > 0 )
    {
        return ret;
    }

    int index = indexedFlags_.size();
    posMap_.insert( str, index );
    posTable_.push_back( str );
    indexedFlags_.push_back( true );

    return index;
}

/*
int POSTable::getCodeFromStr(const std::string& pos)
{
    int ret = posMap_.search( pos.c_str() );
    if( ret > 0 )
    	return ret;
    else
    	return addPOS( pos );
}
*/

const char* POSTable::getStrFromCode(int index) const
{
    if(index < 0 || indexedFlags_.size() <= (size_t)index )
    	return EMPTY;

    return posTable_[index];
}

int POSTable::size() const
{
    //assert(posTable_.size() == posMap_.size() && "POS table size should equal to POS map size");
    return indexedFlags_.size_;
}

int POSTable::getCodeFromType(POSType type) const
{
    assert(type >= 0 && static_cast<unsigned int>(type) < typeTable_.size());

    return typeTable_[type];
}

bool POSTable::isIndexPOS( int posCode ) const
{
	// Error POS Index return true by default
	//if(posCode < 0 || indexedFlags_.size() <= (size_t)posCode )
	//	return true;
	return indexedFlags_[ posCode ];
}

bool POSTable::setIndexPOS( int posCode, bool isIndex )
{
	if(posCode < 0 || indexedFlags_.size() <= (size_t)posCode )
        return false;
	return indexedFlags_[ posCode ] = isIndex;
}

void POSTable::resetIndexPOSList( bool defVal )
{
	for( size_t i = 0; i < indexedFlags_.size(); ++i )
	{
	    indexedFlags_[ i ] = defVal;
	}
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

