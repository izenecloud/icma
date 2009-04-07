/** \file pos_table.cpp
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Apr 02, 2009
 */

#include "pos_table.h"

#include <cassert>

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

POSTable* POSTable::instance_;

POSTable* POSTable::instance()
{
    if(instance_ == 0)
    {
	instance_ = new POSTable;
    }

    return instance_;
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
    posTable_.push_back(pos);

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
    if(index < 0 || static_cast<unsigned int>(index) >= posTable_.size())
	return 0;

    return posTable_[index].c_str();
}

int POSTable::size() const
{
    assert(posTable_.size() == posMap_.size() && "POS table size should equal to POS map size");
    return posTable_.size();
}
} // namespace cma
