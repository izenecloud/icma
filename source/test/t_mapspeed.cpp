/**
 * \file t_mapspeed.cpp
 * \brief 
 * \date Jul 26, 2010
 * \author Vernkin Chen
 */
#include "VTimer.h"
#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <time.h>
#include <VTrie.h>
#include <icma/util/DCTrie.h>

using namespace std;
using namespace cma;

int main( int argc, char** argv )
{
    const char* posFile = "../db/icwb/utf8/pos.pos";

    int testCount = 100000;
    if( argc > 1 )
    {
        int tmpCount = atoi( argv[1] );
        if( tmpCount > 0 )
            testCount = tmpCount;
    }
    int retVal;

    cout << "load POS source ...";
    vector< string > posVec;
    ifstream fin( posFile );
    string line;
    while( fin.eof() == false )
    {
        getline( fin, line );
        if( line.empty() == true )
            continue;
        posVec.push_back( line );
    }
    fin.close();
    size_t posSize = posVec.size();

    cout << "Generate Test Set ( " << testCount << " ) ...";
    vector< string > testVec( testCount );
    srand ( time(NULL) );
    for( int i = 0; i < testCount; ++i )
    {
        testVec[ i ] = posVec[ rand() % posSize ];
    }

    VTimer timer;

    cout << "Test std::map ..." << endl;
    timer.startWithTitle( "Initialize std::map" );
    map< string, int > posMap;
    for( size_t i = 0; i < posSize; ++i )
        posMap[ posVec[i] ] = 1;
    timer.endAndPrint();

    timer.startWithTitle( "Search with std::map" );
    for( int i = 0; i < testCount; ++i )
    {
        //cout << "search " << testVec[i] << endl;
        map< string, int >::iterator itr = posMap.find( testVec[i] );
        if( itr != posMap.end() )
            retVal = itr->second;
    }
    timer.endAndPrint();

    cout << "Test VTrie ..." << endl;
    timer.startWithTitle( "Initialize VTrie" );
    VTrie posTrie;
    for( size_t i = 0; i < posSize; ++i )
    {
        VTrieNode node;
        node.setData( 1 );
        posTrie.insert( posVec[i].c_str(), &node );
    }
    timer.endAndPrint();

    timer.startWithTitle( "Search with VTrie" );
    for( int i = 0; i < testCount; ++i )
    {
        VTrieNode node;
        posTrie.search( testVec[i].c_str(), &node );
    }
    timer.endAndPrint();

    cout << "Test DCTrie ..." << endl;
    timer.startWithTitle( "Initialize DCTrie" );
    DCTrie posDCTrie;

    for( size_t i = 0; i < posSize; ++i )
    {
        posDCTrie.insert( posVec[i].c_str(), (int)i );
    }
    timer.endAndPrint();

    timer.startWithTitle( "Search with DCTrie" );
    for( int i = 0; i < testCount; ++i )
    {
        retVal = posDCTrie.search( testVec[i].c_str() );
        //cout << "search " << testVec[i] << " with " << retVal << endl;
    }
    timer.endAndPrint();
}
