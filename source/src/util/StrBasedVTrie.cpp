/*
 * \file StrBasedVTrie.cpp
 * \brief 
 * \date May 4, 2010
 * \author Vernkin Chen
 */

#include "icma/util/StrBasedVTrie.h"

namespace cma
{

bool createVTrie( const vector<string>& words, VTrie& trie )
{
    VTrieNode node;
    node.data = 1;
    for( vector<string>::const_iterator itr = words.begin(); itr != words.end(); ++itr )
    {
        trie.insert( itr->data(), &node );
    }
    return true;
}


StrBasedVTrie::StrBasedVTrie( VTrie* pTrie )
    : trie(pTrie),
    completeSearch(false)
{
}


void StrBasedVTrie::reset()
{
    completeSearch = true;
    node.init();
}


bool StrBasedVTrie::search( const char *p )
{
    if(!completeSearch)
        return false;

    while(node.moreLong && *p)
    {
        trie->find(*p, &node);
        //cout<<"finding,"<<node<<endl;
        ++p;
    }

    //the node.data can be negative (as no pos tags)
    completeSearch = !(*p) && (node.data > 0 || node.moreLong);

    return completeSearch;
}


bool StrBasedVTrie::firstSearch( const char* p )
{
    reset();
    return search( p );
}


bool StrBasedVTrie::exists()
{
    return completeSearch && node.data > 0;
}

}
