/**
 * \file t_speed.cpp
 * \brief 
 * \date Jul 30, 2010
 * \author Vernkin Chen
 */

#include "VTimer.h"
#include <fstream>
#include <iostream>

// for testTokenizer
#include <icma/util/CPPStringUtils.h>
#include <icma/util/StringArray.h>

using namespace std;
using namespace cma;


void testStringArrayTokenizer()
{
    do
    {
        cout << "Input ('x' to exit): ";
        string line;
        getline( cin, line );
        if( line == "x" || line == "X" )
            break;
        StringArray strArray;
        StringArray::tokenize( line.c_str(), strArray );
        cout << "size " << strArray.size() << ": ";
        for( size_t i = 0; i < strArray.size(); ++i )
            cout << strArray[ i ] << " / ";
        cout << endl;

    }while( true );
}

void testTokenizerSpeed( int argc, char** argv )
{
    const char* testFile = "../db/icwb/utf8/sys.dic";

    if( argc > 1 )
    {
        testFile = argv[1];
    }

    VTimer timer;

    {
        ifstream fin( testFile );
        string line;
        cout << "Test CPPStringUtils::token_string(in, ret) ..." << endl;
        timer.startWithTitle( "CPPStringUtils::token_string(in, ret)" );
        while( fin.eof() == false )
        {
            getline( fin, line );
            vector<string> vec;
            TOKEN_STR( line, vec );
        }
        timer.endAndPrint();
        fin.close();
    }

    {
        ifstream fin( testFile );
        string line;
        cout << "Test StringArray::tokenize(in, ret) ..." << endl;
        timer.startWithTitle( "StringArray::tokenize(in, ret)" );
        while( fin.eof() == false )
        {
            getline( fin, line );
            StringArray strArray;
            StringArray::tokenize( line.c_str(), strArray );
        }
        timer.endAndPrint();
        fin.close();
    }
}

int main( int argc, char** argv )
{
    //testStringArrayTokenizer();
    testTokenizerSpeed( argc, argv );

    return 0;
}
