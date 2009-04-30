/** \file pos_table.cpp
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Apr 02, 2009
 */

#include "pos_table.h"

#include <cassert>
#include <fstream>

using namespace std;

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

bool POSTable::loadConfig(const char* fileName)
{
    // open file
    assert(fileName);
    ifstream from(fileName);
    if(!from) {
        return false;
    }

    // read file, which consists of lines in the format "tag = value"
    string line, tag, value;
    string::size_type i, j, k;
    const char* whitespaces = " \t";

    while(getline(from, line))
    {
        line = line.substr(0, line.find('\r'));

        // ignore the comment start with '#'
        if(!line.empty() && line[0] != '#')
        {
            // set k as the position of '='
            i = line.find_first_not_of(whitespaces);
            if(i == string::npos)
            {
                break;
            }

            k = line.find_first_of('=', i);
            if(k == string::npos || k == 0)
            {
                break;
            }

            // set tag
            j = line.find_last_not_of(whitespaces, k-1);
            if(j == string::npos)
            {
                break;
            }
            tag = line.substr(i, j-i+1);

            // set value
            i = line.find_first_not_of(whitespaces, k+1);
            if(i == string::npos)
            {
                break;
            }

            j = line.find_last_not_of(whitespaces);
            if(j == string::npos)
            {
                value = line.substr(i);
            }
            else
            {
                value = line.substr(i, j-i+1);
            }

            TypeMap::const_iterator iter = typeMap_.find(tag);
            if(iter != typeMap_.end())
            {
                int code = addPOS(value);
                typeTable_[iter->second] = code;
            }
        }
    }


    return true;
}

int POSTable::getCodeFromType(POSType type) const
{
    assert(type >= 0 && static_cast<unsigned int>(type) < typeTable_.size());

    return typeTable_[type];
}

} // namespace cma

